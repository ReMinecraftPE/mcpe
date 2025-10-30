/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/
#include <fstream>
#include <sstream>

#include "AppPlatform_android.hpp"
#include "CustomSoundSystem.hpp"
#include "common/Logger.hpp"
#include "client/player/input/Mouse.hpp"

#include "stb_image.h"
#include "stb_image_write.h"


AppPlatform_android::AppPlatform_android()
{
	m_UserInputStatus = -1;

	m_bIsFocused = false;
	m_bGrabbedMouse = false;
	m_bActuallyGrabbedMouse = false;
	m_bWasUnfocused = false;
	m_bShiftPressed = false;
	m_bIsKeyboardShown = false;

	m_MouseDiffX = 0, m_MouseDiffY = 0;

	m_pSoundSystem = nullptr;
}

AppPlatform_android::~AppPlatform_android()
{
	SAFE_DELETE(m_pSoundSystem);
}

void AppPlatform_android::initConsts()
{
	// just assume an 854x480 window for now:
	m_ScreenWidth  = C_DEFAULT_SCREEN_WIDTH;
	m_ScreenHeight = C_DEFAULT_SCREEN_HEIGHT;
}

int AppPlatform_android::checkLicense()
{
	// we own the game!!
	return 1;
}

void AppPlatform_android::buyGame()
{
}

bool AppPlatform_android::hasFileSystemAccess()
{
	return true;
}

void AppPlatform_android::setExternalStoragePath(const std::string& path)
{
	m_storageDir = path;
}

void AppPlatform_android::saveScreenshot(const std::string& fileName, int width, int height)
{
	std::string saveName = m_storageDir + "/" + fileName;

	LOG_I("Saving in %s", saveName.c_str());

	int npixels = width * height;
	uint32_t* pixels = new uint32_t[npixels];
	glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

	stbi_flip_vertically_on_write(true);

	stbi_write_png(saveName.c_str(), width, height, 4, pixels, width * 4);

	delete[] pixels;
}

int AppPlatform_android::getScreenWidth() const
{
	return m_ScreenWidth;
}

int AppPlatform_android::getScreenHeight() const
{
	return m_ScreenHeight;
}

std::vector<std::string> AppPlatform_android::getUserInput()
{
	return m_UserInput;
}

int AppPlatform_android::getUserInputStatus()
{
	return m_UserInputStatus;
}

void AppPlatform_android::createUserInput()
{
	m_UserInput.clear();
	m_UserInputStatus = -1;

	switch (m_DialogType)
	{
		case DLG_CREATE_WORLD:
		{
			// some placeholder for now
			m_UserInput.push_back("New World");
			m_UserInput.push_back("123456");
			m_UserInputStatus = 1;
			break;
		}
	}
}

void AppPlatform_android::showDialog(eDialogType type)
{
	m_DialogType = type;
}

std::string AppPlatform_android::getDateString(int time)
{
	const time_t t = time_t(time);
	struct tm tf;
	struct tm* tp = gmtime_r(&t, &tf);
	char buffer[128];
	strftime(buffer, sizeof buffer, "%d/%m/%y %H:%M", tp);
	return std::string(buffer);
}

Texture AppPlatform_android::loadTexture(const std::string& str, bool bIsRequired)
{
	std::string realPath = str;
	if (realPath.size() && realPath[0] == '/')
		// trim it off
		realPath = realPath.substr(1);

	AAsset* asset = AAssetManager_open(m_app->activity->assetManager, str.c_str(), AASSET_MODE_BUFFER);
	if (!asset) {
		LOG_E("File %s couldn't be opened", realPath.c_str());
		assert(!bIsRequired && "Hey, a texture couldn't be loaded");
		return Texture(0, 0, nullptr, 1, 0);
	}
	size_t cnt = AAsset_getLength(asset);
	unsigned char* buffer = (unsigned char*)calloc(cnt, sizeof(unsigned char));
	AAsset_read(asset, (void*)buffer, cnt);
	AAsset_close(asset);

	int width = 0, height = 0, channels = 0;

	stbi_uc* img = stbi_load_from_memory(buffer, cnt, &width, &height, &channels, STBI_rgb_alpha);
	if (!img)
	{
		LOG_E("File %s couldn't be loaded via stb_image", realPath.c_str());
		assert(!bIsRequired && "Hey, a texture couldn't be loaded");
		return Texture(0, 0, nullptr, 1, 0);
	}

	free(buffer);
	return Texture(width, height, (uint32_t*)img, 1, 0);
}

SoundSystem* const AppPlatform_android::getSoundSystem() const
{
	return m_pSoundSystem;
}

void AppPlatform_android::initSoundSystem()
{
	if (!m_pSoundSystem)
		m_pSoundSystem = new SOUND_SYSTEM();
	else
		LOG_E("Trying to initialize SoundSystem more than once!");
}

bool AppPlatform_android::isTouchscreen() const
{
	return true;
}

/*
std::vector<std::string> AppPlatform_android::getOptionStrings()
{
	std::vector<std::string> o;

	//o.push_back("mp_username");
	//o.push_back("iProgramInCpp");

	std::ifstream ifs("options.txt");
	if (!ifs.is_open())
		return o;

	std::string str;
	while (true)
	{
		if (!std::getline(ifs, str, '\n'))
			break;

		if (str.empty() || str[0] == '#')
			continue;

		std::stringstream ss;
		ss << str;

		std::string key, value;
		if (std::getline(ss, key, '|') && std::getline(ss, value))
		{
			o.push_back(key);
			o.push_back(value);
		}
	}

	return o;
}

void AppPlatform_android::setOptionStrings(const std::vector<std::string>& str)
{
	assert(str.size() % 2 == 0);

	std::ofstream os("options.txt");

	os << "#Config file for Minecraft PE.  The # at the start denotes a comment, removing it makes it a command.\n\n";
	
	for (int i = 0; i < int(str.size()); i += 2)
		os << str[i] << '|' << str[i + 1] << '\n';
}
*/
void AppPlatform_android::setScreenSize(int width, int height)
{
	m_ScreenWidth = width;
	m_ScreenHeight = height;
}

void AppPlatform_android::initAndroidApp(android_app* ptr)
{
	m_app = ptr;
}

void AppPlatform_android::recenterMouse()
{

}

void AppPlatform_android::setMouseGrabbed(bool b)
{
	m_bGrabbedMouse = b;

	if (m_bActuallyGrabbedMouse == (b && m_bIsFocused))
		return;


}

void AppPlatform_android::getMouseDiff(int& x, int& y)
{
	x = m_MouseDiffX;
	y = m_MouseDiffY;
}

void AppPlatform_android::clearDiff()
{
	m_MouseDiffX = m_MouseDiffY = 0;
}

void AppPlatform_android::updateFocused(bool focused)
{
	m_bIsFocused = focused;
	setMouseGrabbed(m_bGrabbedMouse);
}

bool AppPlatform_android::shiftPressed()
{
	return m_bShiftPressed;
}

void AppPlatform_android::setShiftPressed(bool b)
{
	m_bShiftPressed = b;
}

void AppPlatform_android::showKeyboard(int x, int y, int w, int h)
{
	changeKeyboardVisibility(true);
}

void AppPlatform_android::hideKeyboard()
{
	changeKeyboardVisibility(false);
}

void AppPlatform_android::changeKeyboardVisibility(bool bShown)
{
	JavaVM* pVM = m_app->activity->vm;
	JNIEnv* pEnv = m_app->activity->env;

	// This is horrible. However, I refuse to introduce J*va into my code.
	// Stolen from https://stackoverflow.com/questions/5864790/how-to-show-the-soft-keyboard-on-native-activity

	pVM->AttachCurrentThread(&pEnv, NULL);

	jint flags = 0;

	// Retrieves NativeActivity.
	jobject lNativeActivity = m_app->activity->clazz;
	jclass ClassNativeActivity = pEnv->GetObjectClass(lNativeActivity);

	// Retrieves Context.INPUT_METHOD_SERVICE.
	jclass ClassContext = pEnv->FindClass("android/content/Context");
	jfieldID FieldINPUT_METHOD_SERVICE = pEnv->GetStaticFieldID(ClassContext, "INPUT_METHOD_SERVICE", "Ljava/lang/String;");
	jobject INPUT_METHOD_SERVICE = pEnv->GetStaticObjectField(ClassContext, FieldINPUT_METHOD_SERVICE);

	// Runs getSystemService(Context.INPUT_METHOD_SERVICE).
	jclass ClassInputMethodManager = pEnv->FindClass("android/view/inputmethod/InputMethodManager");
	jmethodID MethodGetSystemService = pEnv->GetMethodID(ClassNativeActivity, "getSystemService","(Ljava/lang/String;)Ljava/lang/Object;");
	jobject lInputMethodManager = pEnv->CallObjectMethod(lNativeActivity, MethodGetSystemService,INPUT_METHOD_SERVICE);

	// Runs getWindow().getDecorView().
	jmethodID MethodGetWindow = pEnv->GetMethodID(ClassNativeActivity, "getWindow","()Landroid/view/Window;");
	jobject lWindow = pEnv->CallObjectMethod(lNativeActivity,MethodGetWindow);
	jclass ClassWindow = pEnv->FindClass("android/view/Window");
	jmethodID MethodGetDecorView = pEnv->GetMethodID(ClassWindow, "getDecorView", "()Landroid/view/View;");
	jobject lDecorView = pEnv->CallObjectMethod(lWindow,MethodGetDecorView);

	if (bShown)
	{
		// Runs lInputMethodManager.showSoftInput(...).
		jmethodID MethodShowSoftInput = pEnv->GetMethodID(ClassInputMethodManager, "showSoftInput","(Landroid/view/View;I)Z");
		jboolean lResult = pEnv->CallBooleanMethod(lInputMethodManager, MethodShowSoftInput,lDecorView, flags);
		
		m_bIsKeyboardShown = lResult;
	}
	else
	{
		// Runs lWindow.getViewToken()
		jclass ClassView = pEnv->FindClass("android/view/View");
		jmethodID MethodGetWindowToken = pEnv->GetMethodID(ClassView, "getWindowToken", "()Landroid/os/IBinder;");
		jobject lBinder = pEnv->CallObjectMethod(lDecorView,MethodGetWindowToken);

		// lInputMethodManager.hideSoftInput(...).
		jmethodID MethodHideSoftInput = pEnv->GetMethodID(ClassInputMethodManager, "hideSoftInputFromWindow","(Landroid/os/IBinder;I)Z");
		jboolean lResult = pEnv->CallBooleanMethod(lInputMethodManager, MethodHideSoftInput,lBinder, flags);

		m_bIsKeyboardShown = false; // just treat it as hidden anyways why not
	}
	pVM->DetachCurrentThread();
}

int AppPlatform_android::getKeyboardUpOffset()
{
	// @TODO
	// For now we'll just return 1/2 of the screen height. That ought to cover most cases.
	return m_ScreenHeight / 2;
}

AssetFile AppPlatform_android::readAssetFile(const std::string& str, bool quiet) const
{
	std::string realPath = str;
	if (realPath.size() && realPath[0] == '/')
		// trim it off
		realPath = realPath.substr(1);

	AAsset* asset = AAssetManager_open(m_app->activity->assetManager, str.c_str(), AASSET_MODE_BUFFER);
	if (!asset) {
		if (!quiet) LOG_E("File %s couldn't be opened", realPath.c_str());
		return AssetFile();
	}
	size_t cnt = AAsset_getLength(asset);
	unsigned char* buffer = new unsigned char[cnt];
	AAsset_read(asset, (void*)buffer, cnt);
	AAsset_close(asset);
	return AssetFile(ssize_t(cnt), buffer);
}