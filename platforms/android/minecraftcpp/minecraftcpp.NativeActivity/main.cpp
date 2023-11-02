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

static void evalJoyStick(engine* engine, AInputEvent* event)
{
    float lx = mapStick(event, AMOTION_EVENT_AXIS_X);
    float ly = mapStick(event, AMOTION_EVENT_AXIS_Y);
    float rx = mapStick(event, AMOTION_EVENT_AXIS_Z);
    float ry = mapStick(event, AMOTION_EVENT_AXIS_RZ);

    if (!engine->ninecraftApp)
    {
        return;
    }
    if (!engine->ninecraftApp->m_pLocalPlayer)
    {
        return;
    }
    if (!engine->ninecraftApp->m_pLocalPlayer->m_pMinecraft)
    {
        return;
    }

    //use controller input on android for now.
    //field_B00 and field_B04 is normally set in LocalPlayer.cpp but is blocked out on android.
    engine->ninecraftApp->m_pLocalPlayer->field_B00 = -lx;
    engine->ninecraftApp->m_pLocalPlayer->field_B04 = -ly;
    //Controller::feed(1, 1, lx, ly); does nothing... doing this above for now
    Controller::feed(2, 1, rx, ry);

    bool curL = mapTrigger(event, AMOTION_EVENT_AXIS_LTRIGGER) > 0.3f;
    bool curR = mapTrigger(event, AMOTION_EVENT_AXIS_RTRIGGER) > 0.3f;
    bool lTrigger = curL;
    bool rTrigger = curR;

    if (s_lastL != curL)
    {
        Mouse::feed(BUTTON_RIGHT, lTrigger, g_MousePosX, g_MousePosY);
    }
    if (s_lastR != curR)
    {
        Mouse::feed(BUTTON_LEFT, rTrigger, g_MousePosX, g_MousePosY);
    }

    s_lastR = curR;
    s_lastL = curL;

}

static int32_t evalKeyInput(struct engine* engine, AInputEvent* event)
{
    int32_t action = AKeyEvent_getAction(event);
    int32_t code = AKeyEvent_getKeyCode(event);

    switch (action)
    {
    case AKEY_EVENT_ACTION_DOWN:
    {
        Keyboard::feed(Keyboard::KeyState::DOWN, code);
        return 1;
    }
    case AKEY_EVENT_ACTION_UP:
    {
        Keyboard::feed(Keyboard::KeyState::UP, code);
        return 1;
    }
    }

    return 0;
}

#define COMPARE_SOURCE(v, s) ((v & s) == s)

static int32_t evalMotionInput(struct engine* engine, AInputEvent* event, int32_t source)
{
    if (COMPARE_SOURCE(source, AINPUT_SOURCE_GAMEPAD) || COMPARE_SOURCE(source, AINPUT_SOURCE_JOYSTICK))
    {
        evalJoyStick(engine, event);
        return 1;
    }

    int32_t action = AMotionEvent_getAction(event);
    unsigned int flags = action & AMOTION_EVENT_ACTION_MASK;

    switch (flags)
    {
    case AMOTION_EVENT_ACTION_HOVER_MOVE:
    {
        g_MousePosX = AMotionEvent_getX(event, 0);
        g_MousePosY = AMotionEvent_getY(event, 0);

        Mouse::feed(BUTTON_NONE, false, g_MousePosX, g_MousePosY);

        return 1;
    }
    case AMOTION_EVENT_ACTION_MOVE:
    {
        int32_t idx = (action & AMOTION_EVENT_ACTION_POINTER_INDEX_MASK) >>
            AMOTION_EVENT_ACTION_POINTER_INDEX_SHIFT;

        g_MousePosX = AMotionEvent_getX(event, idx);
        g_MousePosY = AMotionEvent_getY(event, idx);

        Mouse::feed(BUTTON_NONE, false, g_MousePosX, g_MousePosY);

        return 1;
    }
    case AMOTION_EVENT_ACTION_POINTER_DOWN:
    case AMOTION_EVENT_ACTION_DOWN:
    {
        int32_t idx = (action & AMOTION_EVENT_ACTION_POINTER_INDEX_MASK) >>
            AMOTION_EVENT_ACTION_POINTER_INDEX_SHIFT;

        g_MousePosX = AMotionEvent_getX(event, idx);
        g_MousePosY = AMotionEvent_getY(event, idx);

        Mouse::feed(BUTTON_LEFT, true, g_MousePosX, g_MousePosY);

        return 1;
    }
    case AMOTION_EVENT_ACTION_UP:
    case AMOTION_EVENT_ACTION_POINTER_UP:
    {
        int32_t idx = (action & AMOTION_EVENT_ACTION_POINTER_INDEX_MASK) >>
            AMOTION_EVENT_ACTION_POINTER_INDEX_SHIFT;

        g_MousePosX = AMotionEvent_getX(event, idx);
        g_MousePosY = AMotionEvent_getY(event, idx);

        Mouse::feed(BUTTON_LEFT, false, g_MousePosX, g_MousePosY);

        return 1;
    }
    }


    return 0;
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
    engine->ninecraftApp->sizeUpdate(w, h);

    // initialize the app
    engine->ninecraftApp->init();

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
            {
                break;
            }
            engine.ninecraftApp->update();
            eglSwapBuffers(engine.display, engine.surface);
        }
    }
    engine.ninecraftApp->saveOptions();


    delete engine.ninecraftApp;
}