/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "thirdparty/GL/GL.hpp"
#include "client/app/AppPlatform.hpp"

#include "client/player/input/Mouse.hpp"
#include "client/player/input/Keyboard.hpp"
#include "common/Utils.hpp"
#include "LoggerWin32.hpp"
#include "CustomSoundSystem.hpp"

class AppPlatform_win32 : public AppPlatform
{
public:
	AppPlatform_win32();
	~AppPlatform_win32();

	void initSoundSystem() override;

	void buyGame() override;
	void saveScreenshot(const std::string& fileName, int width, int height) override;
	int checkLicense() override;
	void createUserInput() override;
	std::vector<std::string> getUserInput() override { return m_UserInput; }
	int getUserInputStatus() override { return m_UserInputStatus; }
	int getScreenWidth() const override { return m_ScreenWidth; }
	int getScreenHeight() const override { return m_ScreenHeight; }
	void showDialog(eDialogType) override;
	std::string getDateString(int time) override;
	Texture loadTexture(const std::string& str, bool bIsRequired) override;

	// From v0.1.1. Also add these to determine touch screen use within the game.
	bool isTouchscreen() override;

	// Also add these to allow proper turning within the game.
	void recenterMouse() override;
	void setMouseGrabbed(bool b) override;
	void getMouseDiff(int& x, int& y) override;
	void clearDiff() override;
	void updateFocused(bool focused) override;

	// Also add these to allow proper text input within the game.
	bool shiftPressed() override { return m_bShiftPressed; }
	void setShiftPressed(bool b) { m_bShiftPressed = b; }

	bool hasFileSystemAccess() override;

	// Also add this to allow dynamic texture patching.
	AssetFile readAssetFile(const std::string&) const override;

	void setScreenSize(int width, int height);
	const char* const getWindowTitle() const { return m_WindowTitle; }
	SoundSystem* const getSoundSystem() const override { return m_pSoundSystem; }

	static MouseButtonType GetMouseButtonType(UINT iMsg);
	static bool GetMouseButtonState(UINT iMsg, WPARAM wParam);
	static Keyboard::KeyState GetKeyState(UINT iMsg);

private:
	const char* m_WindowTitle;
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

	int m_MouseDiffX, m_MouseDiffY;

	SOUND_SYSTEM* m_pSoundSystem;
};

