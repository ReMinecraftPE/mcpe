/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "thirdparty/GL/GL.hpp"
#include <ctime>
#include "client/app/AppPlatform.hpp"
#include "source/common/Utils.hpp"
#include "android_native_app_glue.h"

// note: probably won't add AppPlatform_android until it's time
// to build an Android app

class AppPlatform_android : public AppPlatform
{
public:
	AppPlatform_android();
	~AppPlatform_android();
	void initConsts();
	void buyGame() override;
	void saveScreenshot(const std::string& fileName, int width, int height) override;
	int checkLicense() override;
	void createUserInput() override;
	std::vector<std::string> getUserInput() override;
	int getUserInputStatus() override;
	int getScreenWidth() const override;
	int getScreenHeight() const override;
	void showDialog(eDialogType) override;
	std::string getDateString(int time) override;
	Texture loadTexture(const std::string& str, bool b) override;
	//std::vector<std::string>  getOptionStrings() override;

	// Also add these to allow proper turning within the game.
	void recenterMouse() override;
	void setMouseGrabbed(bool b) override;
	void getMouseDiff(int& x, int& y) override;
	void clearDiff() override;
	void updateFocused(bool focused) override;

	// Also add these to allow proper text input within the game.
	bool shiftPressed() override;
	void setShiftPressed(bool b);
	void showKeyboard(int x, int y, int w, int h) override;
	void hideKeyboard() override;
	int getKeyboardUpOffset() override;
	
	// Also add these to allow saving options.
	//void setOptionStrings(const std::vector <std::string>& str) override;
	bool hasFileSystemAccess() override;	

	SoundSystem* const getSoundSystem() const override;
	void initSoundSystem() override;
	bool isTouchscreen() const override;

	void setScreenSize(int width, int height);
	void initAndroidApp(android_app* ptr);
	void setExternalStoragePath(const std::string& path);

	AssetFile readAssetFile(const std::string&, bool) const override;

private:
	void changeKeyboardVisibility(bool bShown);

	int m_ScreenWidth;
	int m_ScreenHeight;

	std::vector<std::string> m_UserInput;
	int m_UserInputStatus;

	eDialogType m_DialogType;

	bool m_bIsFocused;
	bool m_bGrabbedMouse;
	bool m_bActuallyGrabbedMouse;
	bool m_bWasUnfocused;
	bool m_bShiftPressed;
	bool m_bIsKeyboardShown;

	int m_MouseDiffX, m_MouseDiffY;

	android_app* m_app;

	SoundSystem* m_pSoundSystem;
	
	std::string m_storageDir;
};

