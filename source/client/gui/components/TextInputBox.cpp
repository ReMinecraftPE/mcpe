/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "TextInputBox.hpp"
#include "client/app/Minecraft.hpp"
#ifndef ORIGINAL_CODE

TextInputBox::TextInputBox(Screen* parent, int id, int x, int y, int width, int height, const std::string& placeholder, const std::string& text)
{
	m_ID = id;
	m_xPos = x;
	m_yPos = y;
	m_width = width;
	m_height = height;
	m_placeholder = placeholder;
	m_text = text;
	m_bFocused = false;
	m_bEnabled = true;
	m_bCursorOn = true;
	m_insertHead = 0;
	m_lastFlashed = 0;
	m_pFont = nullptr;
	m_pParent = parent;
	m_maxLength = -1;
	m_scrollPos = 0;
}

TextInputBox::~TextInputBox()
{
	m_pParent->m_pMinecraft->platform()->hideKeyboard();
}

void TextInputBox::init(Font* pFont)
{
	m_pFont = pFont;
}

void TextInputBox::setEnabled(bool bEnabled)
{
	m_bEnabled = true;
}

void TextInputBox::keyPressed(int key)
{
	if (!m_bFocused)
	{
		return;
	}

#ifdef USE_SDL
#define AKEYCODE_FORWARD_DEL SDLVK_DELETE
#define AKEYCODE_ARROW_LEFT  SDLVK_LEFT
#define AKEYCODE_ARROW_RIGHT SDLVK_RIGHT
#define AKEYCODE_DEL	         SDLVK_BACKSPACE
#define AKEYCODE_ENTER       SDLVK_RETURN
#elif defined(_WIN32)
#define AKEYCODE_FORWARD_DEL VK_DELETE
#define AKEYCODE_ARROW_LEFT  VK_LEFT
#define AKEYCODE_ARROW_RIGHT VK_RIGHT
#define AKEYCODE_DEL	         VK_BACK
#define AKEYCODE_ENTER       VK_RETURN
#endif
	switch (key) {
		case AKEYCODE_DEL:
		{
			// Backspace
			if (m_text.empty())
			{
				return;
			}
			if (m_insertHead <= 0)
			{
				return;
			}
			if (m_insertHead > int(m_text.size()))
			{
				m_insertHead = int(m_text.size());
			}
			m_text.erase(m_text.begin() + m_insertHead - 1, m_text.begin() + m_insertHead);
			m_insertHead--;
			recalculateScroll();
			break;
		}
		case AKEYCODE_FORWARD_DEL:
		{
			// Delete
			if (m_text.empty())
			{
				return;
			}
			if (m_insertHead < 0)
			{
				return;
			}
			if (m_insertHead >= int(m_text.size()))
			{
				return;
			}
			m_text.erase(m_text.begin() + m_insertHead, m_text.begin() + m_insertHead + 1);
			break;
		}
		case AKEYCODE_ARROW_LEFT:
		{
			// Left
			m_insertHead--;
			if (m_insertHead < 0)
			{
				m_insertHead = 0;
			}
			recalculateScroll();
			break;
		}
		case AKEYCODE_ARROW_RIGHT:
		{
			// Right
			m_insertHead++;
			if (!m_text.empty())
			{
				if (m_insertHead > int(m_text.size()))
				{
					m_insertHead = int(m_text.size());
				}
			}
			else
			{
				m_insertHead = 0;
			}
			recalculateScroll();
			break;
		}
		case AKEYCODE_ENTER:
		{
			// Enter
			m_bFocused = false;
			break;
		}
	}
#ifdef AKEYCODE_FORWARD_DEL
#undef AKEYCODE_FORWARD_DEL
#undef AKEYCODE_ARROW_LEFT
#undef AKEYCODE_ARROW_RIGHT
#undef AKEYCODE_DEL
#undef AKEYCODE_ENTER
#endif
#ifdef AKEYCODE_DEL
#undef AKEYCODE_DEL
#endif
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

	if (b)
	{
		int x = (int) (((float) m_xPos) / Gui::InvGuiScale);
		int y = (int) (((float) m_yPos) / Gui::InvGuiScale);
		int w = (int) (((float) m_width) / Gui::InvGuiScale);
		int h = (int) (((float) m_height) / Gui::InvGuiScale);
		m_pParent->m_pMinecraft->platform()->showKeyboard(x, y, w, h);
	}
	else
	{
		m_pParent->m_pMinecraft->platform()->hideKeyboard();
	}

	m_bFocused = b;
	if (b)
	{
		m_lastFlashed = getTimeMs();
		m_bCursorOn = true;
		m_insertHead = int(m_text.size());
		recalculateScroll();
	}

	// don't actually hide the keyboard when unfocusing
	// - we may be undoing the work of another text box
}

void TextInputBox::onClick(int x, int y)
{
	setFocused(clicked(x, y));
}

void TextInputBox::charPressed(int k)
{
	if (!m_bFocused)
	{
		return;
	}

	// Ignore Unprintable Characters
	if (k == '\n' || k < ' ' || k > '~')
	{
		return;
	}

	// Check Max Length
	if (m_maxLength != -1 && int(m_text.length()) >= m_maxLength)
	{
		return;
	}

	// Insert
	m_text.insert(m_text.begin() + m_insertHead, k);
	m_insertHead++;
	recalculateScroll();
}

constexpr int PADDING = 5;

std::string TextInputBox::getRenderedText(int scroll_pos, std::string text)
{
	// Not the most efficient code.
	// But it does not run often enough to matter.
	std::string rendered_text = text.substr(scroll_pos);
	int max_width = m_width - (PADDING * 2);
	while (m_pFont->width(rendered_text) > max_width)
	{
		rendered_text.pop_back();
	}
	return rendered_text;
}

constexpr char CURSOR_CHAR = '_';

void TextInputBox::render()
{
	fill(m_xPos, m_yPos, m_xPos + m_width, m_yPos + m_height, 0xFFAAAAAA);
	fill(m_xPos + 1, m_yPos + 1, m_xPos + m_width - 1, m_yPos + m_height - 1, 0xFF000000);

	int text_color;
	int scroll_pos;
	std::string rendered_text;
	if (m_text.empty())
	{
		rendered_text = m_placeholder;
		text_color = 0x404040;
		scroll_pos = 0;
	}
	else
	{
		rendered_text = m_text;
		text_color = 0xffffff;
		scroll_pos = m_scrollPos;
	}
	rendered_text = getRenderedText(scroll_pos, rendered_text);

	int textYPos = (m_height - 8) / 2;
	drawString(m_pFont, rendered_text, m_xPos + PADDING, m_yPos + textYPos, text_color);

	if (m_bCursorOn)
	{
		int cursor_pos = m_insertHead - m_scrollPos;
		if (cursor_pos >= 0 && cursor_pos <= int(rendered_text.length()))
		{
			std::string substr = rendered_text.substr(0, cursor_pos);
			int xPos = PADDING + m_pFont->width(substr);

			std::string str;
			str += CURSOR_CHAR;
			drawString(m_pFont, str, m_xPos + xPos, m_yPos + textYPos + 2, 0xffffff);
		}
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

void TextInputBox::recalculateScroll()
{
	// Skip If Size Unset
	if (m_width == 0)
	{
		return;
	}
	// Ensure Cursor Is Visible
	bool is_cursor_at_end = m_insertHead == int(m_text.length());
	if (m_scrollPos >= m_insertHead && m_scrollPos > 0)
	{
		// Cursor Is At Scroll Position
		// Move Back Scroll As Far As Possible
		while (true)
		{
			int test_scroll_pos = m_scrollPos - 1;
			std::string rendered_text = m_text;
			if (is_cursor_at_end)
			{
				rendered_text += CURSOR_CHAR;
			}
			rendered_text = getRenderedText(test_scroll_pos, rendered_text);
			int cursor_pos = m_insertHead - test_scroll_pos;
			if (cursor_pos >= int(rendered_text.length()))
			{
				break;
			}
			else
			{
				m_scrollPos = test_scroll_pos;
				if (m_scrollPos == 0)
				{
					break;
				}
			}
		}
	}
	else
	{
		// Cursor After Scroll Area
		// Increase Scroll So Cursor Is Visible
		while (true)
		{
			std::string rendered_text = m_text;
			if (is_cursor_at_end)
			{
				rendered_text += CURSOR_CHAR;
			}
			rendered_text = getRenderedText(m_scrollPos, rendered_text);
			int cursor_pos = m_insertHead - m_scrollPos;
			if (cursor_pos < int(rendered_text.length()))
			{
				break;
			}
			else
			{
				if (m_scrollPos == int(m_text.length())) {
					LOG_W("Text Box Is Too Small");
					break;
				} else {
					m_scrollPos++;
				}
			}
		}
	}
}

std::string TextInputBox::getText()
{
	return m_text;
}

void TextInputBox::setText(std::string str)
{
	m_text = str;
	m_insertHead = int(m_text.size());
}

bool TextInputBox::isFocused()
{
	return m_bFocused;
}

void TextInputBox::setMaxLength(int max_length)
{
	m_maxLength = max_length;
}

#endif
