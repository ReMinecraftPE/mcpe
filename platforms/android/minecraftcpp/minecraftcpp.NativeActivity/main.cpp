#include <jni.h>
#include <errno.h>
#include "android_native_app_glue.h"
#include <string.h>
#include <unistd.h>
#include <sys/resource.h>
#include <thread>
#include <android/sensor.h>
#include <android/log.h>

#include "compat/KeyCodes.hpp"
#include "thirdparty/GL/GL.hpp"
#include "platforms/android/AppPlatform_android.hpp"
#include "client/app/NinecraftApp.hpp"
#include "client/gui/screens/ProgressScreen.hpp"
#include "client/player/input/Controller.hpp"
#include "client/player/input/Mouse.hpp"
#include "client/player/input/Multitouch.hpp"


AppPlatform_android g_AppPlatform;

bool g_LButtonDown, g_RButtonDown;
int g_MousePosX, g_MousePosY;

struct engine
{
    struct android_app* androidApp;
    EGLDisplay display = nullptr;
    EGLSurface surface = nullptr;
    EGLContext context = nullptr;
    int animating = 0;
    NinecraftApp* ninecraftApp = nullptr;
};


static float mapStick(AInputEvent* event, int32_t axis)
{
    const float deadZone = .265f;
    float value = AMotionEvent_getAxisValue(event, axis, 0);
    if (value > deadZone)
        return (value - deadZone) / (1.f - deadZone);
    else if (value < -deadZone)
        return (value + deadZone) / (1.f - deadZone);
    else
        return 0.f;
}

static float mapTrigger(AInputEvent* event, int32_t axis)
{
    const float deadZone = .1f;
    float value = AMotionEvent_getAxisValue(event, axis, 0);
    if (value > deadZone)
        return (value - deadZone) / (1.f - deadZone);
    else
        return 0.f;
}

static bool s_lastR = false;
static bool s_lastL = false;

static int evalKeyInput(struct engine* engine, AInputEvent* event)
{
    bool keyDown = AKeyEvent_getAction(event) == AKEY_EVENT_ACTION_DOWN;
    int  keyCode = AKeyEvent_getKeyCode(event);

    bool isBack = true;

    if (keyCode != AKEYCODE_BACK || AInputEvent_getDeviceId(event) != 0)
        isBack = false;

    int repeatCount = AKeyEvent_getRepeatCount(event);
    if (repeatCount <= 0 && !isBack)
        Keyboard::feed(keyDown ? Keyboard::UP : Keyboard::DOWN, keyCode);

    if (keyCode == AKEYCODE_BACK)
    {
        if (repeatCount == 0)
            engine->ninecraftApp->handleBack(keyDown);

        return 1;
    }

    // let Android handle those
    if (keyCode >= AKEYCODE_VOLUME_UP && keyCode <= AKEYCODE_VOLUME_DOWN)
        return 0;

    return 1;
}

#define COMPARE_SOURCE(v, s) ((v & s) == s)

static void nativeMouseDown(int id, int x, int y)
{
    Mouse::feed(BUTTON_LEFT, true, x, y);
    Multitouch::feed(BUTTON_LEFT, true, x, y, id);
}

static void nativeMouseUp(int id, int x, int y)
{
    Mouse::feed(BUTTON_LEFT, false, x, y);
    Multitouch::feed(BUTTON_LEFT, false, x, y, id);
}

static void nativeMouseMove(int id, int x, int y)
{
    Mouse::feed(BUTTON_NONE, false, x, y);
    Multitouch::feed(BUTTON_NONE, false, x, y, id);
}

static int32_t evalMotionInput(struct engine* engine, AInputEvent* event, int32_t source)
{
    size_t pointerCount = AMotionEvent_getPointerCount(event);

    //! NOTE: Android documentation clearly states that pointerCount is always >=1.
    if (pointerCount <= 0)
        return 1;

    int actionp = AMotionEvent_getAction(event);
    int action = actionp & AMOTION_EVENT_ACTION_MASK;
    int pointerId = AMotionEvent_getPointerId(event, (actionp & AMOTION_EVENT_ACTION_POINTER_INDEX_MASK) >> AMOTION_EVENT_ACTION_POINTER_INDEX_SHIFT);

    int x = AMotionEvent_getX(event, pointerId);
    int y = AMotionEvent_getY(event, pointerId);

    switch (action)
    {
        case AMOTION_EVENT_ACTION_DOWN:
        case AMOTION_EVENT_ACTION_POINTER_DOWN:
            nativeMouseDown(pointerId, x, y);
            break;
        case AMOTION_EVENT_ACTION_UP:
        case AMOTION_EVENT_ACTION_POINTER_UP:
            nativeMouseUp(pointerId, x, y);
            break;
        case AMOTION_EVENT_ACTION_MOVE:
        {
            int pc = AMotionEvent_getPointerCount(event);
            for (int i = 0; i < pc; i++)
            {
                nativeMouseMove(AMotionEvent_getPointerId(event, i),
                                AMotionEvent_getX(event, i),
                                AMotionEvent_getY(event, i));
            }
            break;
        }
    }

    return 1;
}

static int32_t engine_handle_input(struct android_app* app, AInputEvent* event)
{
    struct engine* engine = (struct engine*)app->userData;

    int32_t type = AInputEvent_getType(event);
    int32_t source = AInputEvent_getSource(event);

    switch (type)
    {
        case AINPUT_EVENT_TYPE_KEY:
            return evalKeyInput(engine, event);
        case AINPUT_EVENT_TYPE_MOTION:
            return evalMotionInput(engine, event, source);
    }

    return 0;
}

/**
* Process the next main command.
*/

static void initWindow(struct engine* engine, struct android_app* app)
{
    if (engine->androidApp->window == NULL)
    {
        LOG_E("no active window?");
        return;
    }

    EGLint w, h, format;
    //EGLint numConfigs;
    engine->display = eglGetDisplay(EGL_DEFAULT_DISPLAY);

    if (!eglInitialize(engine->display, 0, 0))
    {
        LOG_E("eglInitialize failed %i", eglGetError());
        return;
    }

    EGLConfig config;

    EGLint attribs[] = {
    EGL_SURFACE_TYPE, EGL_OPENGL_ES2_BIT,
    EGL_DEPTH_SIZE, 0x10,
    EGL_RENDERABLE_TYPE, EGL_VERSION_1_3,
    EGL_NONE
    };

    EGLint numConfigs = 0;
    if (!(eglChooseConfig(engine->display, attribs, &config, 1, &numConfigs) && numConfigs > 0))
    {
        LOG_E("eglChooseConfig failed %i", eglGetError());
        return;
    }


    eglGetConfigAttrib(engine->display, config, EGL_NATIVE_VISUAL_ID, &format);

    ANativeWindow_setBuffersGeometry(app->window, 0, 0, format);

    engine->surface = eglCreateWindowSurface(engine->display, config, app->window, NULL);
    engine->context = eglCreateContext(engine->display, config, NULL, NULL);

    if (!engine->context)
    {
        LOG_E("cant create egl context %x", eglGetError());
        return;
    }

    if (eglMakeCurrent(engine->display, engine->surface, engine->surface, engine->context) == EGL_FALSE)
    {
        LOG_E("Unable to eglMakeCurrent");
        return;
    }

    eglQuerySurface(engine->display, engine->surface, EGL_WIDTH, &w);
    eglQuerySurface(engine->display, engine->surface, EGL_HEIGHT, &h);

    g_AppPlatform.initConsts();
    g_AppPlatform.setScreenSize(w, h);
    g_AppPlatform.initAndroidApp(app);

    engine->ninecraftApp = new NinecraftApp;
    engine->ninecraftApp->m_pPlatform = &g_AppPlatform;
    engine->ninecraftApp->m_externalStorageDir = ".";
    engine->ninecraftApp->width = w;
    engine->ninecraftApp->height = h;

    // initialize the app
    engine->ninecraftApp->init();
    engine->ninecraftApp->sizeUpdate(w, h);

    LOG_I("finished initializing");
    engine->animating = 1; 
}

static void engine_handle_cmd(struct android_app* app, int32_t cmd) {
    struct engine* engine = (struct engine*)app->userData;
    switch (cmd) {
    case APP_CMD_CONFIG_CHANGED:
        LOG_I("APP_CMD_CONFIG_CHANGED");

        break;
    case APP_CMD_SAVE_STATE:
        //ninecraftApp saveState not implemented do nothing
        break;
    case APP_CMD_INIT_WINDOW:
        LOG_I("APP_CMD_INIT_WINDOW");
        initWindow(engine, app);
        break;

    case APP_CMD_TERM_WINDOW:
        LOG_I("APP_CMD_TERM_WINDOW");
        if (engine->display)
        {
            eglMakeCurrent(engine->display, 0, 0, 0);
            if (engine->context)
                eglDestroyContext(engine->display, engine->context);
            if (engine->surface)
                eglDestroySurface(engine->display, engine->surface);
            eglTerminate(engine->display);
        }

        break;
    case APP_CMD_GAINED_FOCUS:
        engine->animating = 1;
        break;
    case APP_CMD_LOST_FOCUS:
        engine->animating = 0;
        break;
    }
}

void android_main(struct android_app* state) {
    struct engine engine;

    memset(&engine, 0, sizeof(engine));
    state->userData = &engine;
    state->onAppCmd = engine_handle_cmd;
    state->onInputEvent = engine_handle_input;
    engine.androidApp = state;


    while (1)
    {
        int ident;
        int events;
        struct android_poll_source* source;

        while ((ident = ALooper_pollAll(engine.animating ? 0 : -1, NULL, &events,
            (void**)&source)) >= 0) {

            if (source != NULL) {
                source->process(state, source);
            }
        }

        if (engine.animating)
        {
            if (engine.ninecraftApp->wantToQuit())
                break;

            engine.ninecraftApp->update();
            eglSwapBuffers(engine.display, engine.surface);
        }
    }

    engine.ninecraftApp->saveOptions();
    delete engine.ninecraftApp;
}