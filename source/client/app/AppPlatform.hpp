/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include <string>
#include <vector>
#include <map>

#include "client/renderer/Texture.hpp"
#include "client/renderer/texture/TextureData.hpp"
#include "client/sound/SoundSystem.hpp"
#include "AssetFile.hpp"

#include "GameMods.hpp"
#include "compat/PlatformDefinitions.h"

#ifdef MOD_USE_BIGGER_SCREEN_SIZE
#define C_DEFAULT_SCREEN_WIDTH  (1280)
#define C_DEFAULT_SCREEN_HEIGHT (720)
#elif defined(__DREAMCAST__)
#define C_DEFAULT_SCREEN_WIDTH  (800)
#define C_DEFAULT_SCREEN_HEIGHT (600)
#elif MC_PLATFORM_XBOX360
#define C_DEFAULT_SCREEN_WIDTH  (1280)
#define C_DEFAULT_SCREEN_HEIGHT (720)
#else
#define C_DEFAULT_SCREEN_WIDTH  (854)
#define C_DEFAULT_SCREEN_HEIGHT (480)
#endif

#define C_HOME_PATH "/games/com.mojang/"
#define C_MAX_LOCAL_PLAYERS 4

struct MessageModal
{
	enum Type
	{
		TYPE_INFO,
		TYPE_ERROR
	};

	Type type;
	std::string text;

	MessageModal(Type type, const std::string& text)
		: type(type)
		, text(text)
	{
	}
};


class GameControllerHandler;
class AppPlatformListener;

class AppPlatform
{
public:
	typedef std::multimap<float, AppPlatformListener*> ListenerMap;

public:
	enum eDialogType
	{
		DLG_NONE,
		DLG_CREATE_WORLD,
		DLG_CHAT,
		DLG_OPTIONS,
		DLG_RENAME_MP_WORLD
	};

private:
	static AppPlatform* m_singleton;
public:
	static AppPlatform* singleton();

public:
	AppPlatform();
	virtual ~AppPlatform();

private:
	virtual void _tick();

public:
	virtual void buyGame();
	virtual int checkLicense();
	virtual void createUserInput();
	virtual void finish();
	virtual std::string getDateString(int);
	virtual int getScreenWidth() const;
	virtual int getScreenHeight() const;
	virtual std::vector<std::string> getUserInput();
	virtual int getUserInputStatus();
	virtual bool hasBuyButtonWhenInvalidLicense();
	virtual void saveScreenshot(const std::string&, int, int);
	virtual void showDialog(eDialogType);
	virtual void uploadPlatformDependentData(int, void*);
	virtual void loadImage(ImageData& data, const std::string& path);
	TextureData loadTexture(const std::string& path);
	virtual bool doesTextureExist(const std::string& path) const;
	// From v0.1.1. Also add these to determine touch screen use within the game.
	virtual bool isTouchscreen() const;
	virtual bool hasGamepad() const;
	virtual GameControllerHandler* getGameControllerHandler();
	// Also add these to allow proper turning within the game.
	virtual void recenterMouse();
	virtual void setMouseGrabbed(bool b);
	virtual void getMouseDiff(int& x, int& y);
	virtual void clearDiff();
	virtual void updateFocused(bool focused);
	// Also add this to allow proper text input within the game.
	virtual bool controlPressed();
	virtual bool shiftPressed();
	// On-screen keyboard
	virtual void showKeyboard(int x, int y, int w, int h);
	virtual void showKeyboard();
	virtual void showKeyboard(bool bShown); // @TODO: Why on earth is this here?
	// virtual void showKeyboard(std::string const &currentText, int maxLength, bool limitInput);
	virtual void hideKeyboard();
	virtual void onHideKeyboard(); // called by the runner, not the game
  #ifdef USE_NATIVE_ANDROID
	virtual int getKeyboardUpOffset();
  #endif
	virtual void vibrate(int milliSeconds);
    virtual bool getRecenterMouseEveryTick();
	virtual std::string getClipboardText();
	
	void _fireLowMemory();
	void _fireAppSuspended();
	void _fireAppResumed();
	void _fireAppFocusLost();
	void _fireAppFocusGained();
	void _fireAppTerminated();

	virtual bool hasFileSystemAccess();
	virtual void initSoundSystem();
	virtual SoundSystem* getSoundSystem() const;
	// Used For Sounds
	virtual std::string getAssetPath(const std::string& path) const;
	virtual std::string getExternalStoragePath(const std::string& path) const;
	virtual bool hasAssetFile(const std::string& path) const;
	virtual AssetFile readAssetFile(const std::string& path, bool quiet) const;
	virtual std::string readAssetFileStr(const std::string& path, bool quiet) const;
	virtual void makeNativePath(std::string& path) const;

	// For getting a handle on the save device for consoles
	virtual void beginProfileDataRead(unsigned int playerId);
	virtual void endProfileDataRead(unsigned int playerId);
	virtual void beginProfileDataWrite(unsigned int playerId);
	virtual void endProfileDataWrite(unsigned int playerId);

	virtual void showMessageModal(const struct MessageModal& msg);

public:
	ListenerMap m_listeners;
	std::string m_externalStorageDir;
	void* m_hWnd; // the Mojang solution
};

