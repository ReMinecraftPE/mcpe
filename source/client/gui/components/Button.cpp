/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "Button.hpp"
#include "renderer/ShaderConstants.hpp"

void Button::_init()
{
	m_color = Color::WHITE;

#ifndef ORIGINAL_CODE
	m_lastX = 0;
	m_lastY = 0;
#endif
}

void Button::_renderBg(Minecraft* mc, const MenuPointer& pointer)
{
	int iYPos = 20 * getYImage(isSelected()) + 46;
	mc->m_pTextures->loadAndBindTexture("gui/gui.png");
	blit(m_xPos, m_yPos, 0, iYPos, m_width / 2, m_height, 0, 20, &m_materials.ui_textured_and_glcolor);
	blit(m_xPos + m_width / 2, m_yPos, 200 - m_width / 2, iYPos, m_width / 2, m_height, 0, 20, &m_materials.ui_textured_and_glcolor);
}

void Button::_renderBgPocket(Minecraft* mc, const MenuPointer& pointer)
{
	int iXPos = isSelected() ? 66 : 0;
	currentShaderColor = isEnabled() ? Color::WHITE : Color::GREY;
	mc->m_pTextures->loadAndBindTexture("gui/touchgui.png");
	blit(m_xPos, m_yPos, iXPos, 0, m_width, m_height, 66, 26, &m_materials.ui_textured_and_glcolor);
}

void Button::_renderBgConsole(Minecraft* mc, const MenuPointer& pointer)
{
	Textures& texs = *mc->m_pTextures;

	if (!isEnabled())
		currentShaderColor.a *= 0.5f;
	blitSprite(texs, isSelected() && !hasFocus() ? "gui/console/Graphics/MainMenuButton_Over.png" : "gui/console/Graphics/MainMenuButton_Norm.png", m_xPos, m_yPos, m_width, m_height, &m_materials.ui_textured_and_glcolor);
	if (hasFocus())
	{
		float timer = (getTimeMs() % 1200) / 1200.0f;
		currentShaderColor.a *= 0.5f + (timer >= 0.5f ? 1 - timer : timer);
		blitSprite(texs, "gui/console/Graphics/MainMenuButton_Over.png", m_xPos, m_yPos, m_width, m_height, &m_materials.ui_textured_and_glcolor);
		currentShaderColor = m_color;
	}
}

void Button::_renderMessage(Font& font)
{
	Color textColor;
	if (!isEnabled())
		textColor = Color(160, 160, 160, m_color.a); // 0xFFA0A0A0
	else if (isSelected())
		textColor = Color(255, 255, 160, m_color.a); // 0xFFFFA0U
	else
		textColor = Color(224, 224, 224, m_color.a); // 0xE0E0E0U
	drawCenteredString(font, getMessage(), m_xPos + m_width / 2, m_yPos + (m_height - 8) / 2, textColor);
}

void Button::_renderMessageConsole(Font& font)
{
	Color textColor;
	if (hasFocus())
	{
		float timer = (getTimeMs() % 1200) / 1200.0f;
		textColor = Color(220, 220, Mth::round((0.5f - (timer >= 0.5f ? 1 - timer : timer)) * 220), currentShaderColor.a);
	}
	else if (isSelected())
	{
		textColor = Color(220, 220, 0, currentShaderColor.a); // 0xDCDC00
	}
	else
		textColor = Color(224, 224, 224, currentShaderColor.a); // 0xE0E0E0U
	int textWidth = font.width(getMessage()) * 2;
	font.drawScalableShadow(getMessage(), m_xPos + (m_width - textWidth) / 2, m_yPos + (m_height - 16) / 2, textColor);
}

Button::Button(int xPos, int yPos, int btnWidth, int btnHeight, const std::string& text)
{
	_init();

	m_xPos = xPos;
	m_yPos = yPos;
	setMessage(text);
	m_width  = btnWidth;
	m_height = btnHeight;
}

Button::Button(int xPos, int yPos, const std::string& text)
{
	_init();

	m_xPos = xPos;
	m_yPos = yPos;
	setMessage(text);
	m_width  = 200;
	m_height = 24;
}

Button::Button(const std::string& text)
{
	_init();

	setMessage(text);
	m_width  = 200;
	m_height = 24;
}

int Button::getYImage(bool bHovered)
{
	if (!isEnabled()) return 0;
	if (bHovered) return 2;
	return 1;
}

void Button::renderBg(Minecraft* pMinecraft, const MenuPointer& pointer)
{
	switch (m_uiTheme)
	{
	case UI_POCKET:
		_renderBgPocket(pMinecraft, pointer);
		break;
	case UI_CONSOLE:
		_renderBgConsole(pMinecraft, pointer);
		break;
	default:
		_renderBg(pMinecraft, pointer);
		break;
	}
}

void Button::render(Minecraft* pMinecraft, const MenuPointer& pointer)
{
	if (!isVisible()) return;

	if (!pMinecraft->m_pScreen->doElementTabbing())
		setSelected(isHovered(pMinecraft, pointer));

	if (m_color.a == 0.0f)
		return;

	currentShaderColor = m_color;
	renderBg(pMinecraft, pointer);

	Font& font = *pMinecraft->m_pFont;

	if (m_uiTheme == UI_CONSOLE)
		_renderMessageConsole(font);
	else
		_renderMessage(font);
}
