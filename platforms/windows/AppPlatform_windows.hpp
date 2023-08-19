/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include <ctime>

#include "compat/GL.hpp"
#include "AppPlatform.hpp"

#include "client/player/input/Mouse.hpp"
#include "client/player/input/Keyboard.hpp"
#include "client/common/Utils.hpp"

class AppPlatform_windows : public AppPlatform
{
public:
	AppPlatform_windows();
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
	Texture loadTexture(const std::string& str, bool b) override;
	std::vector<std::string> getOptionStrings() override;

	// Also add these to allow proper turning within the game.
	void recenterMouse() override;
	void setMouseGrabbed(bool b) override;
	void getMouseDiff(int& x, int& y) override;
	void clearDiff() override;
	void updateFocused(bool focused) override;

	// Also add these to allow proper text input within the game.
	bool shiftPressed() override { return m_bShiftPressed; }
	void setShiftPressed(bool b) { m_bShiftPressed = b; }
	
	// Also add these to allow saving options.
	void setOptionStrings(const std::vector <std::string>& str) override;

	// Also add this to allow dynamic texture patching.
	std::string getPatchData() override;

	void setScreenSize(int width, int height);
	const char* const getWindowTitle() const { return m_WindowTitle; }

	static Mouse::ButtonType GetMouseButtonType(UINT iMsg);
	static Mouse::ButtonState GetMouseButtonState(UINT iMsg, WPARAM wParam);
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
};

