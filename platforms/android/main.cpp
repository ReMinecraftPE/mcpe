/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include <jni.h>
#include <errno.h>
#include "android_native_app_glue.h"
#include <string.h>
#include <unistd.h>
#include <sys/resource.h>
#include <thread>
#include <android/sensor.h>
#include <android/log.h>

#include "common/Logger.hpp"
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
    bool initted = false;
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

static char getCharFromKey(int32_t keyCode, int32_t metaState)
{
    bool bShiftPressed = metaState & AMETA_SHIFT_ON;

    // well you can't really press alt-X or ctrl-X and expect an actual character
    if (metaState & (AMETA_ALT_ON | AMETA_CTRL_ON))
        return '\0';

    // Alphabet
    if (keyCode >= AKEYCODE_A && keyCode <= AKEYCODE_Z)
        return char((keyCode - AKEYCODE_A) + (bShiftPressed ? 'A' : 'a'));

    // Digits
    if (keyCode >= AKEYCODE_0 && keyCode <= AKEYCODE_9)
    {
        static const char* shiftmap = ")!@#$%^&*(";
        return char(bShiftPressed ? shiftmap[keyCode - AKEYCODE_0] : (keyCode - AKEYCODE_0 + '0'));
    }

    // NumPad
    if (keyCode >= AKEYCODE_NUMPAD_0 && keyCode <= AKEYCODE_NUMPAD_9)
        return char(keyCode + '0' - AKEYCODE_NUMPAD_0);

    switch (keyCode)
    {
        case AKEYCODE_DEL:           return '\b';
        case AKEYCODE_FORWARD_DEL:   return '\001';
        case AKEYCODE_ARROW_LEFT:    return '\002';
        case AKEYCODE_ARROW_RIGHT:   return '\003';
        case AKEYCODE_SPACE:         return ' ';
        case AKEYCODE_COMMA:         return bShiftPressed ? '<' : ',';
        case AKEYCODE_PERIOD:        return bShiftPressed ? '>' : '.';
        case AKEYCODE_EQUALS:        return bShiftPressed ? '+' : '=';
        case AKEYCODE_MINUS:         return bShiftPressed ? '_' : '-';
        case AKEYCODE_SEMICOLON:     return bShiftPressed ? ':' : ';';
        case AKEYCODE_SLASH:         return bShiftPressed ? '?' : '/';
        case AKEYCODE_GRAVE:         return bShiftPressed ? '~' : '`';
        case AKEYCODE_BACKSLASH:     return bShiftPressed ? '|' : '\\';
        case AKEYCODE_APOSTROPHE:    return bShiftPressed ? '"' : '\'';
        case AKEYCODE_LEFT_BRACKET:  return bShiftPressed ? '{' : '[';
        case AKEYCODE_RIGHT_BRACKET: return bShiftPressed ? '}' : ']';
    }

    return '\0';
}

static int evalKeyInput(struct engine* engine, AInputEvent* event)
{
    bool keyDown = AKeyEvent_getAction(event) == AKEY_EVENT_ACTION_DOWN;
    int  keyCode = AKeyEvent_getKeyCode(event);

    bool isBack = true;

    if (keyCode != AKEYCODE_BACK || AInputEvent_getDeviceId(event) != 0)
        isBack = false;

    int repeatCount = AKeyEvent_getRepeatCount(event);
    if (repeatCount <= 0 && !isBack)
        Keyboard::feed(keyDown ? Keyboard::DOWN : Keyboard::UP, keyCode);

    // We define HANDLE_CHARS_SEPARATELY but have to fake it
    if (keyDown) {
        char chr = getCharFromKey(keyCode, AKeyEvent_getMetaState(event));
        if (chr != '\0')
            engine->ninecraftApp->handleCharInput(chr);
    }
    
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
    int pointerIndex = (actionp & AMOTION_EVENT_ACTION_POINTER_INDEX_MASK) >> AMOTION_EVENT_ACTION_POINTER_INDEX_SHIFT;
    int pointerId = AMotionEvent_getPointerId(event, pointerIndex);

    int x = AMotionEvent_getX(event, pointerIndex);
    int y = AMotionEvent_getY(event, pointerIndex);

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

static std::string getExternalStorageDir(struct engine* engine)
{
    // Normally we would use the external storage directory. However, late Android is being
    // very bitchy about giving us the permission. And that's probably a good thing, actually.

    // This returns a directory path that looks something like the following, which is still
    // exposed to the user, therefore we get the benefits of having it exposed, aside from the
    // fact that removing the app will delete all your worlds.
    // /.../Android/data/com.minecraftcpp/files
#ifndef USE_EXTERNAL_STORAGE

    return std::string(engine->androidApp->activity->externalDataPath);

#else

    ANativeActivity* pActivity = engine->androidApp->activity;
    JNIEnv* pJNIEnv = pActivity->env;

    pActivity->vm->AttachCurrentThread(&pJNIEnv, nullptr);

    jclass Environment = pJNIEnv->FindClass("android/os/Environment");
    jmethodID GetDirId = pJNIEnv->GetStaticMethodID(Environment, "getExternalStorageDirectory", "()Ljava/io/File;");

    if (pJNIEnv->ExceptionOccurred())
        pJNIEnv->ExceptionDescribe();

    // Since it takes a String
    jobject     FileObject = pJNIEnv->CallStaticObjectMethod(Environment, GetDirId);
    jclass      FileClass  = pJNIEnv->GetObjectClass(FileObject);
    jmethodID   AbsPathId  = pJNIEnv->GetMethodID(FileClass, "getAbsolutePath", "()Ljava/lang/String;");
    jobject     PathObject = pJNIEnv->CallObjectMethod(FileObject, AbsPathId);
    const char* PathString = pJNIEnv->GetStringUTFChars((jstring) PathObject, nullptr);
    std::string result(PathString);

    pJNIEnv->ReleaseStringUTFChars((jstring) PathObject, PathString);
    pActivity->vm->DetachCurrentThread();

    return result;

#endif
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

    engine->ninecraftApp->width = w;
    engine->ninecraftApp->height = h;

    if (!engine->initted)
    {
        engine->ninecraftApp->m_externalStorageDir = getExternalStorageDir(engine);
        g_AppPlatform.setExternalStoragePath(engine->ninecraftApp->m_externalStorageDir);
        engine->ninecraftApp->init();
    }
    else
    {
        engine->ninecraftApp->onGraphicsReset();
    }

    engine->ninecraftApp->sizeUpdate(w, h);

    engine->initted = true;

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
        engine->ninecraftApp->saveOptions();
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
        engine->ninecraftApp->saveOptions();
        break;
    }
}

extern bool g_bIsGrassColorAvailable;	  // world/level/GrassColor.cpp
extern bool g_bIsFoliageColorAvailable;   // world/level/FoliageColor.cpp

static void CheckOptionalTextureAvailability()
{
#ifdef FEATURE_MENU_BACKGROUND
	Screen::setIsMenuPanoramaAvailable(true);
#endif

#ifdef FEATURE_CLOUDS
	LevelRenderer::setAreCloudsAvailable(true);
#endif

#ifdef FEATURE_GRASS_COLOR
	g_bIsGrassColorAvailable = true;
#endif

#ifdef FEATURE_FOLIAGE_COLOR
	g_bIsFoliageColorAvailable = true;
#endif
}


void android_main(struct android_app* state) {
    struct engine engine;
	
	CheckOptionalTextureAvailability();

    memset(&engine, 0, sizeof(engine));
    state->userData = &engine;
    state->onAppCmd = engine_handle_cmd;
    state->onInputEvent = engine_handle_input;
    engine.androidApp = state;

    engine.ninecraftApp = new NinecraftApp;
    engine.ninecraftApp->m_pPlatform = &g_AppPlatform;

    while (1)
    {
        int ident;
        int events;
        struct android_poll_source* source;

        while ((ident = ALooper_pollAll(engine.animating ? 0 : -1, NULL, &events,
            (void**)&source)) >= 0) {

            if (source != NULL)
            {
                source->process(state, source);
            }

            if (state->destroyRequested != 0)
            {
                goto exit;
            }
        }

        if (engine.animating)
        {
            engine.ninecraftApp->update();
            eglSwapBuffers(engine.display, engine.surface);

            if (engine.ninecraftApp->wantToQuit())
            {
                ANativeActivity_finish(state->activity);
            }
        }
    }

 exit:
    engine.ninecraftApp->saveOptions();
    delete engine.ninecraftApp;
}