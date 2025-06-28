/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "client/player/input/Mouse.hpp"
#include "client/player/input/Keyboard.hpp"
#include "components/Button.hpp"
#include "components/TextInputBox.hpp"

class Button;
class TextInputBox;

class Screen : public GuiComponent
{
private:
	static bool _isPanoramaAvailable;
public:
	static bool isMenuPanoramaAvailable()
	{
#ifdef ENH_MENU_BACKGROUND
		return _isPanoramaAvailable;
#else
		return false;
#endif
	}
	static void setIsMenuPanoramaAvailable(bool value) { _isPanoramaAvailable = value; }

public:
	Screen();
	virtual ~Screen();

	void init(Minecraft*, int, int);
	void updateTabButtonSelection();
	void setSize(int width, int height);
	void onRender(int mouseX, int mouseY, float f);
	int getYOffset();

	virtual void render(int, int, float);
	virtual void init() {};
	virtual void updateEvents();
	virtual void mouseEvent();
	virtual void keyboardEvent();
	virtual bool handleBackEvent(bool b) { return false; }
	virtual void tick();
	virtual void removed() {};
	virtual void renderBackground(int);
	virtual void renderBackground();
	virtual void renderDirtBackground(int);
	virtual bool isPauseScreen() { return true; }
	virtual bool isErrorScreen() { return false; }
	virtual bool isInGameScreen() { return true; }
	virtual void confirmResult(bool b, int i) {};
	virtual void onTextBoxUpdated(int id) {};
	virtual void buttonClicked(Button* pButton) {};
	virtual void mouseClicked(int, int, int);
	virtual void mouseReleased(int, int, int);
	virtual void keyPressed(int);
	virtual void keyboardNewChar(char);
	virtual void handleScroll(bool down);

	// ported from 0.8
	virtual void renderMenuBackground(float f);

public:
	int m_width;
	int m_height;
	bool field_10;
	Minecraft* m_pMinecraft;
	std::vector<Button*> m_buttons;
	std::vector<Button*> m_buttonTabList; 
	int m_tabButtonIndex;
	Font* m_pFont;
	Button* m_pClickedButton;

#ifndef ORIGINAL_CODE
	std::vector<TextInputBox*> m_textInputs;
	int m_yOffset;
#endif
};

