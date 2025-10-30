/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "../GuiComponent.hpp"
#include "../Screen.hpp"

class Screen;
class Minecraft;

// @NOTE: This is NOT original Mojang code.

#ifndef ORIGINAL_CODE

// @TODO: Rename this to TextBox and inherit GuiElement
// Don't trash this in favor of Mojang's class, just mold it, since this is better at handling all platforms
class TextInputBox : public GuiComponent
{
private:
	int m_ID;
	std::string m_text;

public:
	TextInputBox(Screen*, int id, int x, int y, int width = 200, int height = 12, const std::string& placeholder = "", const std::string& text = "");
	~TextInputBox();

	void init(Font* pFont);
	void setEnabled(bool bEnabled);
	void keyPressed(int key);
	void charPressed(int chr);
	void render();
	void tick();
	void setFocused(bool b);
	void onClick(int x, int y);
	bool clicked(int x, int y);
	std::string getText();
	void setText(const std::string& text);
	bool isFocused();
	void setMaxLength(int max_length);

	// From TextBox in 0.7.0
	int getKey() const { return m_ID; }
	std::string getText() const { return m_text; }

public:
#ifndef HANDLE_CHARS_SEPARATELY
	char guessCharFromKey(int key);
#endif
	std::string getRenderedText(int scroll_pos, std::string text);
	void recalculateScroll();

	int m_xPos;
	int m_yPos;
	int m_width;
	int m_height;
	std::string m_placeholder;
	bool m_bFocused;
	bool m_bEnabled;
	bool m_bCursorOn;
	int m_insertHead;
	int m_lastFlashed;
	Font* m_pFont;
	Screen* m_pParent;
	int m_maxLength;
	int m_scrollPos;
};

#endif
