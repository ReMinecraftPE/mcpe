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

// @TODO: Rename this to TextBox
// Don't trash this in favor of Mojang's class, just mold it, since this is better at handling all platforms
class TextBox : public GuiElement
{
private:
	std::string m_text;

public:
	TextBox(Screen*, int id, int x, int y, int width = 200, int height = 12, const std::string& placeholder = "", const std::string& text = "");
	~TextBox();

protected:
	void _onFocusChanged() override;

public:
	Type getType() const override { return TYPE_TEXTINPUT; }

private:
	std::string _sanitizePasteText(const std::string& text) const;

public:
	void init(Font* pFont);
	void keyPressed(int key);
	void charPressed(int chr);
	void pasteText(const std::string& text);
	void render();
	void tick();
	void onClick(int x, int y);
	bool clicked(int x, int y);
	void setText(const std::string& text);
	void setMaxLength(int max_length);

	const std::string& getText() const { return m_text; }

public:
#ifndef HANDLE_CHARS_SEPARATELY
	char guessCharFromKey(int key);
#endif
	std::string getRenderedText(int scroll_pos, std::string text);
	void recalculateScroll();

public:
	std::string m_placeholder;
	bool m_bCursorOn;
	int m_insertHead;
	int m_lastFlashed;
	Font* m_pFont;
	Screen* m_pParent;
	int m_maxLength;
	int m_scrollPos;
};

#endif
