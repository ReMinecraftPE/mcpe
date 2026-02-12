/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "../GuiElement.hpp"
#include "../Screen.hpp"
#include "../MenuPointer.hpp"

class Screen;

// @NOTE: This is NOT original Mojang code.

#ifndef ORIGINAL_CODE

class TextBox : public GuiElement
{
private:
	std::string m_text;

public:
	TextBox(Screen*, int x, int y, int width = 200, int height = 12, const std::string& placeholder = "", const std::string& text = "");
	~TextBox();

protected:
	void _onSelectedChanged() override;
	void _onFocusChanged() override;

public:
	Type getType() const override { return TYPE_TEXTBOX; }

private:
	std::string _sanitizePasteText(const std::string& text) const;

public:
	void init(Font* pFont);
	bool pointerPressed(Minecraft* pMinecraft, const MenuPointer& pointer) override;
	void handleButtonPress(Minecraft* pMinecraft, int key) override;
	void handleTextChar(Minecraft* pMinecraft, int chr) override;
	void handleClipboardPaste(const std::string& text) override;
	void render(Minecraft* pMinecraft, const MenuPointer& pointer) override;
	void tick(Minecraft* pMinecraft) override;
	void setTextboxText(const std::string& text) override;
	void setMaxLength(int max_length);
	void setBordered(bool);

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
	bool m_bBordered;
};

#endif
