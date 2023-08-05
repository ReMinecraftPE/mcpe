/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "TextInputBox.hpp"
#include "Minecraft.hpp"
#ifndef ORIGINAL_CODE

TextInputBox::TextInputBox(int id, int x, int y) :
	TextInputBox(id, x, y, 200, 12, "", "")
{
}

TextInputBox::TextInputBox(int id, int x, int y, int width, int height) :
	TextInputBox(id, x, y, width, height, "", "")
{
}

TextInputBox::TextInputBox(int id, int x, int y, int width, int height, const std::string& placeholder) :
	TextInputBox(id, x, y, width, height, placeholder, "")
{
}

TextInputBox::TextInputBox(int id, int x, int y, int width, int height, const std::string& placeholder, const std::string& text) :
	m_ID(id),
	m_xPos(x),
	m_yPos(y),
	m_width(width),
	m_height(height),
	m_placeholder(placeholder),
	m_text(text)
{
}

void TextInputBox::init(Font* pFont)
{
	m_pFont = pFont;
}

void TextInputBox::setEnabled(bool bEnabled)
{
	m_bEnabled = true;
}

void TextInputBox::keyPressed(Minecraft* minecraft, int key)
{
	if (!m_bFocused)
		return;

	bool bShiftPressed = minecraft->platform()->shiftPressed();
	
#ifndef HANDLE_CHARS_SEPARATELY
	char chr = '\0';
	if (key >= AKEYCODE_A && key <= AKEYCODE_Z)
	{
		chr = char((key - AKEYCODE_A) + (bShiftPressed ? 'A' : 'a'));
	}
	if (key >= AKEYCODE_0 && key <= AKEYCODE_9)
	{
		static const char shiftmap[] = { ')', '!', '@', '#', '$', '%', '^', '&', '*', '(' };
		chr = char(bShiftPressed ? shiftmap[key - AKEYCODE_0] : (key - AKEYCODE_0 + '0'));
	}
	switch (key)
	{
		case AKEYCODE_DEL:
			chr = '\b';
			break;
		case AKEYCODE_FORWARD_DEL:
			chr = '\001';
			break;
		case AKEYCODE_ARROW_LEFT:
			chr = '\002';
			break;
		case AKEYCODE_ARROW_RIGHT:
			chr = '\003';
			break;
		case AKEYCODE_SPACE:
			chr = ' ';
			break;
		case AKEYCODE_COMMA:
			chr = bShiftPressed ? '<' : ',';
			break;
		case AKEYCODE_PERIOD:
			chr = bShiftPressed ? '>' : '.';
			break;
		case AKEYCODE_PLUS:
			chr = bShiftPressed ? '+' : '=';
			break;
		case AKEYCODE_MINUS:
			chr = bShiftPressed ? '_' : '-';
			break;
		case AKEYCODE_SEMICOLON:
			chr = bShiftPressed ? ':' : ';';
			break;
		case AKEYCODE_SLASH:
			chr = bShiftPressed ? '?' : '/';
			break;
		case AKEYCODE_GRAVE:
			chr = bShiftPressed ? '~' : '`';
			break;
		case AKEYCODE_BACKSLASH:
			chr = bShiftPressed ? '|' : '\\';
			break;
		case AKEYCODE_APOSTROPHE:
			chr = bShiftPressed ? '"' : '\'';
			break;
		case AKEYCODE_LEFT_BRACKET:
			chr = bShiftPressed ? '{' : '[';
			break;
		case AKEYCODE_RIGHT_BRACKET:
			chr = bShiftPressed ? '}' : ']';
			break;
	}
#else
	char chr = '\0';
	switch (key)
	{
		case AKEYCODE_FORWARD_DEL:
			chr = '\001';
			break;
		case AKEYCODE_ARROW_LEFT:
			chr = '\002';
			break;
		case AKEYCODE_ARROW_RIGHT:
			chr = '\003';
			break;
	}
#endif

	if (chr)
		charPressed(chr);
}

void TextInputBox::tick()
{
	if (!m_lastFlashed)
		m_lastFlashed = getTimeMs();

	if (m_bFocused)
	{
		if (getTimeMs() > m_lastFlashed + 500)
		{
			m_lastFlashed += 500;
			m_bCursorOn ^= 1;
		}
	}
	else
	{
		m_bCursorOn = false;
	}
}

void TextInputBox::setFocused(bool b)
{
	if (m_bFocused == b)
		return;

	m_bFocused = b;
	if (b)
	{
		m_lastFlashed = getTimeMs();
		m_bCursorOn = true;
		m_insertHead = int(m_text.size());
	}
}

void TextInputBox::onClick(int x, int y)
{
	setFocused(clicked(x, y));
}

void TextInputBox::charPressed(int k)
{
	if (!m_bFocused)
		return;

	if (k == '\b')
	{
		if (m_text.empty())
			return;

		if (m_insertHead <= 0)
			return;

		if (m_insertHead > int(m_text.size()))
			m_insertHead = int(m_text.size());

		m_text.erase(m_text.begin() + m_insertHead - 1, m_text.begin() + m_insertHead);
		m_insertHead--;
		return;
	}

	if (k == '\001') // delete
	{
		if (m_text.empty())
			return;

		if (m_insertHead < 0)
			return;

		if (m_insertHead >= int(m_text.size()))
			return;

		m_text.erase(m_text.begin() + m_insertHead, m_text.begin() + m_insertHead + 1);
		return;
	}

	if (k == '\002') // left
	{
		m_insertHead--;
		if (m_insertHead < 0)
			m_insertHead = 0;
	}

	if (k == '\003') // right
	{
		m_insertHead++;
		if (!m_text.empty())
		{
			if (m_insertHead > int(m_text.size()))
				m_insertHead = int(m_text.size());
		}
		else
		{
			m_insertHead = 0;
		}
	}

	if (k == '\n' || k == '\r')
	{
		m_bFocused = false;
		return;
	}

	// other unrenderable character?
	if (k < ' ' || k > '~')
		return;

	// note: the width will increase by the same amount no matter where K is appended
	int width = m_pFont->width(m_text + char(k));
	if (width < m_width - 2)
	{
		m_text.insert(m_text.begin() + m_insertHead, k);
		m_insertHead++;
	}
}

void TextInputBox::render()
{
	fill(m_xPos, m_yPos, m_xPos + m_width, m_yPos + m_height, 0xFFAAAAAA);
	fill(m_xPos + 1, m_yPos + 1, m_xPos + m_width - 1, m_yPos + m_height - 1, 0xFF000000);

	int textYPos = (m_height - 8) / 2;

	if (m_text.empty())
		drawString(m_pFont, m_placeholder, m_xPos + 2, m_yPos + 2, 0x404040);
	else
		drawString(m_pFont, m_text, m_xPos + 2, m_yPos + textYPos, 0xFFFFFF);

	if (m_bCursorOn)
	{
		int xPos = 2;
		
		std::string substr = m_text.substr(0, m_insertHead);
		xPos += m_pFont->width(substr);

		drawString(m_pFont, "_", m_xPos + xPos, m_yPos + textYPos + 2, 0xFFFFFF);
	}
}

bool TextInputBox::clicked(int xPos, int yPos)
{
	if (!m_bEnabled) return false;

	if (xPos < m_xPos) return false;
	if (yPos < m_yPos) return false;
	if (xPos >= m_xPos + m_width) return false;
	if (yPos >= m_yPos + m_height) return false;

	return true;
}

#endif
