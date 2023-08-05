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

class Screen : public GuiComponent
{
public:
	Screen();
	virtual ~Screen();

	void init(Minecraft*, int, int);
	void updateTabButtonSelection();
	void setSize(int width, int height);

	virtual void render(int, int, float);
	virtual void init();
	virtual void updateEvents();
	virtual void mouseEvent();
	virtual void keyboardEvent();
	virtual bool handleBackEvent(bool);
	virtual void tick();
	virtual void removed();
	virtual void renderBackground(int);
	virtual void renderBackground();
	virtual void renderDirtBackground(int);
	virtual bool isPauseScreen();
	virtual bool isErrorScreen();
	virtual bool isInGameScreen();
	virtual void confirmResult(bool, int);
	virtual void buttonClicked(Button*);
	virtual void mouseClicked(int, int, int);
	virtual void mouseReleased(int, int, int);
	virtual void keyPressed(int);
	virtual void charInput(char);

public:
	int m_width = 1;
	int m_height = 1;
	bool field_10 = false;
	Minecraft* m_pMinecraft;
	std::vector<Button*> m_buttons;
	std::vector<Button*> m_buttonTabList; 
	int m_tabButtonIndex = 0;
	Font* m_pFont;
	Button* m_pClickedButton = 0;

#ifndef ORIGINAL_CODE
	std::vector<TextInputBox*> m_textInputs;
#endif
};

