/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include <string>
#include <vector>

#include "client/renderer/Texture.hpp"
#include "client/sound/SoundSystem.hpp"
#include "AssetFile.hpp"

class AppPlatform
{
public:
	enum eDialogType
	{
		DLG_CREATE_WORLD = 1,
		DLG_CHAT,
		DLG_OPTIONS,
		DLG_RENAME_MP_WORLD,
	};

private:
	static AppPlatform* m_singleton;
public:
	static AppPlatform* const singleton();

public:
	AppPlatform();
	virtual ~AppPlatform();

private:
	virtual void _tick();

protected:
	virtual std::string _getPatchDataPath() const { return "patches/patch_data.txt"; }

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
	virtual Texture loadTexture(const std::string&, bool bIsRequired);

#ifndef ORIGINAL_CODE
	virtual bool doesTextureExist(const std::string& path) const;
	// From v0.1.1. Also add these to determine touch screen use within the game.
	virtual bool isTouchscreen() const;
	virtual bool hasGamepad() const;
	// Also add these to allow proper turning within the game.
	virtual void recenterMouse();
	virtual void setMouseGrabbed(bool b);
	virtual void getMouseDiff(int& x, int& y);
	virtual void clearDiff();
	virtual void updateFocused(bool focused);
	// Also add this to allow proper text input within the game.
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
	
	void _fireLowMemory();
	void _fireAppSuspended();
	void _fireAppResumed();
	void _fireAppFocusLost();
	void _fireAppFocusGained();
	void _fireAppTerminated();

	virtual bool hasFileSystemAccess();
	// Also add this to allow dynamic patching.
	virtual std::string getPatchData();
	virtual void initSoundSystem();
	virtual SoundSystem* const getSoundSystem() const;
	// Used For Sounds
	virtual std::string getAssetPath(const std::string& path) const;
	virtual AssetFile readAssetFile(const std::string&, bool) const;
#endif
};

