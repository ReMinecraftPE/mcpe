/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "TextBox.hpp"
#include "common/Logger.hpp"
#include "compat/KeyCodes.hpp"
#ifndef ORIGINAL_CODE

#ifdef USE_NATIVE_ANDROID
#define HANDLE_CHARS_SEPARATELY // faked though, see platforms/android/minecraftcpp/minecraftcpp.NativeActivity/main.cpp
#endif

static bool IsInvalidCharacter(char c)
{
	return c == '\n' || c < ' ' || c > '~';
}

TextBox::TextBox(Screen* parent, int x, int y, int width, int height, const std::string& placeholder, const std::string& text) : GuiElement()
{
	m_xPos = x;
	m_yPos = y;
	m_width = width;
	m_height = height;
	m_placeholder = placeholder;
	m_text = text;
	m_bCursorOn = true;
	m_insertHead = 0;
	m_lastFlashed = 0;
	m_pFont = nullptr;
	m_pParent = parent;
	m_maxLength = -1;
	m_scrollPos = 0;
	m_bBordered = true;
}

TextBox::~TextBox()
{
	m_pParent->m_pMinecraft->platform()->hideKeyboard(0);
}

void TextBox::_onSelectedChanged()
{
	if (isSelected())
	{
		m_lastFlashed = getTimeMs();
		m_bCursorOn = true;
		m_insertHead = int(m_text.size());
		recalculateScroll();
	}
	else if (hasFocus())
	{
		setFocused(false);
	}
}

void TextBox::_onFocusChanged()
{
	if (hasFocus())
	{
		VirtualKeyboard keyboard;
		IntRectangle& rect = keyboard.rect;

		rect.x = (int)(((float)m_xPos) / Gui::GuiScale);
		rect.y = (int)(((float)m_yPos) / Gui::GuiScale);
		rect.w = (int)(((float)m_width) / Gui::GuiScale);
		rect.h = (int)(((float)m_height) / Gui::GuiScale);

		keyboard.defaultText = m_text;

		m_pParent->m_pMinecraft->platform()->showKeyboard(0, keyboard);
	}
	else
	{
		m_pParent->m_pMinecraft->platform()->hideKeyboard(0);
	}

	// don't actually hide the keyboard when unfocusing
	// - we may be undoing the work of another text box
}

void TextBox::init(Font* pFont)
{
	m_pFont = pFont;
}

bool TextBox::pointerPressed(Minecraft* pMinecraft, const MenuPointer& pointer)
{
	bool result = _isHovered(pointer);
	setFocused(result);
	if (result)
	{
		// scuffed as hell
		pMinecraft->m_pScreen->selectElementById(getId());
	}
	return result;
}

#ifdef USE_SDL
#define AKEYCODE_FORWARD_DEL   SDLK_DELETE
#define AKEYCODE_ARROW_LEFT    SDLK_LEFT
#define AKEYCODE_ARROW_RIGHT   SDLK_RIGHT
#define AKEYCODE_DEL           SDLK_BACKSPACE
#define AKEYCODE_ENTER         SDLK_RETURN
#define AKEYCODE_A             SDLK_a
#define AKEYCODE_Z             SDLK_z
#define AKEYCODE_0             SDLK_0
#define AKEYCODE_9             SDLK_9
#define AKEYCODE_SPACE         SDLK_SPACE
#define AKEYCODE_COMMA         SDLK_COMMA
#define AKEYCODE_PERIOD        SDLK_PERIOD
#define AKEYCODE_PLUS          SDLK_PLUS
#define AKEYCODE_MINUS         SDLK_MINUS
#define AKEYCODE_SEMICOLON     SDLK_SEMICOLON
#define AKEYCODE_SLASH         SDLK_SLASH
#define AKEYCODE_GRAVE         SDLK_BACKQUOTE
#define AKEYCODE_BACKSLASH     SDLK_BACKSLASH
#define AKEYCODE_APOSTROPHE    SDLK_QUOTE
#define AKEYCODE_LEFT_BRACKET  SDLK_LEFTBRACKET
#define AKEYCODE_RIGHT_BRACKET SDLK_RIGHTBRACKET
#elif defined(_WIN32)
// See https://learn.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes
#define AKEYCODE_FORWARD_DEL   VK_DELETE
#define AKEYCODE_ARROW_LEFT    VK_LEFT
#define AKEYCODE_ARROW_RIGHT   VK_RIGHT
#define AKEYCODE_DEL               VK_BACK
#define AKEYCODE_ENTER         VK_RETURN
#define AKEYCODE_A             'A'
#define AKEYCODE_Z             'Z'
#define AKEYCODE_0             '0'
#define AKEYCODE_9             '9'
#define AKEYCODE_SPACE         VK_SPACE
#define AKEYCODE_COMMA         VK_OEM_COMMA
#define AKEYCODE_PERIOD        VK_OEM_PERIOD
#define AKEYCODE_PLUS          VK_OEM_PLUS
#define AKEYCODE_MINUS         VK_OEM_MINUS
#define AKEYCODE_SEMICOLON     VK_OEM_1
#define AKEYCODE_SLASH         VK_OEM_2
#define AKEYCODE_GRAVE         VK_OEM_3
#define AKEYCODE_BACKSLASH     VK_OEM_5
#define AKEYCODE_APOSTROPHE    VK_OEM_7
#define AKEYCODE_LEFT_BRACKET  VK_OEM_4
#define AKEYCODE_RIGHT_BRACKET VK_OEM_6
#endif

#ifndef HANDLE_CHARS_SEPARATELY

char TextBox::guessCharFromKey(int key) {
	bool bShiftPressed = m_pParent->m_pMinecraft->platform()->shiftPressed();
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
	return chr;
}

#endif

void TextBox::handleButtonPress(Minecraft* pMinecraft, int key)
{
	Options& options = *pMinecraft->getOptions();

	if (!hasFocus())
	{
		if (options.isKey(KM_MENU_OK, key))
			setFocused(true);
		return;
	}

#ifndef HANDLE_CHARS_SEPARATELY
	char guess = guessCharFromKey(key);
	if (guess != '\0') {
		handleTextChar(guess);
		return;
	}
#endif

	switch (key) {
		case AKEYCODE_DEL:
		{
			// handled elsewhere, do not dupe
		//	handleTextChar('\b');
			break;
		}
		case AKEYCODE_FORWARD_DEL:
		{
			handleTextChar(pMinecraft, '\x7f'); // DELETE
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
			setFocused(false);
			break;
		}
	}
}

void TextBox::tick(Minecraft* pMinecraft)
{
	if (!m_lastFlashed)
		m_lastFlashed = getTimeMs();

	if (isSelected())
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

void TextBox::handleTextChar(Minecraft* pMinecraft, int k)
{
	if (!hasFocus())
		return;

	switch (k) {
		case '\b': // BACKSPACE
		case '\x7f': // DELETE
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
		/* There's not much of a point in handling deletes differently,
		 * especially since old Mac OS versions send delete instead of backspace.
		case '\x7f': // DELETE
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
		}*/
        default:
        {
            // Ignore Unprintable Characters
            if (IsInvalidCharacter(k))
                return;
            
            // Check Max Length
            if (m_maxLength != -1 && int(m_text.length()) >= m_maxLength)
            {
                return;
            }
            
            // Insert
            m_text.insert(m_text.begin() + m_insertHead, k);
            m_insertHead++;
            recalculateScroll();
            break;
        }
    }
	m_pParent->onTextBoxUpdated(getId());
}

void TextBox::handleClipboardPaste(const std::string& text)
{
	if (!hasFocus())
		return;

	const std::string sanitizedText = _sanitizePasteText(text);

	if (!sanitizedText.empty())
	{
		m_text.insert(m_insertHead, sanitizedText);
		m_insertHead += int(sanitizedText.length());
		recalculateScroll();

		m_pParent->onTextBoxUpdated(getId());
	}
}

std::string TextBox::_sanitizePasteText(const std::string& text) const
{
	// check max size, can we add any further text?
	if (m_maxLength != -1 && int(m_text.length()) >= m_maxLength)
		return "";

	// sanitize the text
	std::string sanitized;
	sanitized.reserve(text.length());

	for (size_t i = 0; i < text.length(); ++i)
	{
		char c = text[i];
		if (!IsInvalidCharacter(c))
		{
			sanitized += c;

			// check if we can add any further text
			if (m_maxLength != -1 && int(m_text.length() + sanitized.length()) >= m_maxLength)
				break;
		}
	}

	return sanitized;
}

constexpr int PADDING = 5;

std::string TextBox::getRenderedText(int scroll_pos, std::string text)
{
	// Not the most efficient code.
	// But it does not run often enough to matter.
	std::string rendered_text = text.substr(scroll_pos);
	if (!m_pFont)
		return rendered_text;
	int max_width = m_width - (PADDING * 2);
	while (m_pFont->width(rendered_text) > max_width && !rendered_text.empty())
	{
		//rendered_text.pop_back(); // breaks C++03 compatibility
		rendered_text.erase(rendered_text.end() - 1);
	}
	return rendered_text;
}

constexpr char CURSOR_CHAR = '_';

void TextBox::render(Minecraft* pMinecraft, const MenuPointer& pointer)
{
	if (!m_pFont)
		return;
	if (m_bBordered)
	{
		if (m_uiTheme == UI_CONSOLE)
		{
			blitNineSlice(*pMinecraft->m_pTextures, isSelected() ? "gui/text_field_highlighted.png" : "gui/text_field.png", m_xPos, m_yPos, m_width, m_height, 3);
		}
		else
		{
			// blue: 0xFFE9B3A9
			// button-yellow: 0xFFA0FFFF
			fill(m_xPos, m_yPos, m_xPos + m_width, m_yPos + m_height, isSelected() ? 0xFFA0FFFF : 0xFFAAAAAA);
			fill(m_xPos + 1, m_yPos + 1, m_xPos + m_width - 1, m_yPos + m_height - 1, 0xFF000000);
		}
	}

	Color text_color;
	int scroll_pos;
	std::string rendered_text;
	if (m_text.empty())
	{
		rendered_text = m_placeholder;
		text_color = Color::TEXT_GREY;
		scroll_pos = 0;
	}
	else
	{
		rendered_text = m_text;
		text_color = Color::WHITE;
		scroll_pos = m_scrollPos;
	}
	rendered_text = getRenderedText(scroll_pos, rendered_text);

	int textYPos = (m_height - 8) / 2;
	if (m_uiTheme == UI_CONSOLE)
	{
		textYPos -= 4;
		m_pFont->drawScalable(rendered_text, m_xPos + PADDING * 2, m_yPos + textYPos, text_color);
	}
	else
		drawString(*m_pFont, rendered_text, m_xPos + PADDING, m_yPos + textYPos, text_color);

	if (m_bCursorOn)
	{
		int cursor_pos = m_insertHead - m_scrollPos;
		if (cursor_pos >= 0 && cursor_pos <= int(rendered_text.length()))
		{
			std::string substr = rendered_text.substr(0, cursor_pos);
			int xPos = PADDING + m_pFont->width(substr);

			std::string str;
			str += CURSOR_CHAR;
			if (m_uiTheme == UI_CONSOLE)
				m_pFont->drawScalable(str, m_xPos + xPos * 2, m_yPos + textYPos + 2, Color::WHITE);
			else
				drawString(*m_pFont, str, m_xPos + xPos, m_yPos + textYPos + 2, Color::WHITE);
		}
	}
}

void TextBox::recalculateScroll()
{
	// Skip If Size Unset
	if (m_width == 0)
	{
		return;
	}
	if (!m_pFont)
		return;
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

void TextBox::setTextboxText(const std::string& text)
{
	m_text = text;
	m_insertHead = int(m_text.size());

	recalculateScroll();

	m_pParent->onTextBoxUpdated(getId());
}

void TextBox::setMaxLength(int max_length)
{
	m_maxLength = max_length;
}

void TextBox::setBordered(bool bordered)
{
	m_bBordered = bordered;
}

#endif
