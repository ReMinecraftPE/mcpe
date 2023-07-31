/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "Utils.hpp"
#include "GuiComponent.hpp"

class Minecraft;

// @NOTE: This is NOT original Mojang code.

#ifndef ORIGINAL_CODE

class TextInputBox : public GuiComponent
{
public:
	TextInputBox(int id, int x, int y);
	TextInputBox(int id, int x, int y, int width, int height);
	TextInputBox(int id, int x, int y, int width, int height, const std::string& placeholder);
	TextInputBox(int id, int x, int y, int width, int height, const std::string& placeholder, const std::string& text);

	void init(Font* pFont);
	void setEnabled(bool bEnabled);
	void keyPressed(Minecraft*, int key);
	void charPressed(int chr);
	void render();
	void tick();
	void setFocused(bool b);
	void onClick(int x, int y);
	bool clicked(int x, int y);

public:
	int m_ID = 0;
	int m_xPos = 0;
	int m_yPos = 0;
	int m_width = 0;
	int m_height = 0;
	std::string m_placeholder;
	std::string m_text;
	bool m_bFocused = false;
	bool m_bEnabled = true;
	bool m_bCursorOn = true;
	int m_insertHead = 0;
	int m_lastFlashed = 0;
	Font* m_pFont = nullptr;
};

#endif
