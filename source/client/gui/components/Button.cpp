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
	Textures& texs = *pMinecraft->m_pTextures;

	if (m_uiTheme == UI_CONSOLE)
	{
		if (!isEnabled())
			currentShaderColor.a *= 0.5f;
		blitSprite(texs, isSelected() && !hasFocus() ? "consolegui/Graphics/MainMenuButton_Over.png" : "consolegui/Graphics/MainMenuButton_Norm.png", m_xPos, m_yPos, m_width, m_height);
		if (hasFocus())
		{
			float timer = (getTimeMs() % 1200) / 1200.0f;
			currentShaderColor.a *= 0.5f + (timer >= 0.5f ? 1 - timer : timer);
			blitSprite(texs, "consolegui/Graphics/MainMenuButton_Over.png", m_xPos, m_yPos, m_width, m_height);
			currentShaderColor = m_color;
		}
	}
	else
	{
		int iYPos = 20 * getYImage(isSelected()) + 46;
		texs.loadAndBindTexture("gui/gui.png");
		blit(m_xPos, m_yPos, 0, iYPos, m_width / 2, m_height, 0, 20, &m_materials.ui_textured_and_glcolor);
		blit(m_xPos + m_width / 2, m_yPos, 200 - m_width / 2, iYPos, m_width / 2, m_height, 0, 20, &m_materials.ui_textured_and_glcolor);
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
	{
		Color textColor;
		if (hasFocus())
		{
			float timer = (getTimeMs() % 1200) / 1200.0f;
			textColor = Color(220, 220, Mth::round((0.5f - (timer >= 0.5f ? 1 - timer : timer)) * 220), m_color.a);
		}
		else if (isSelected())
		{
			textColor = Color(220, 220, 0, m_color.a); // 0xDCDC00
		}
		else
			textColor = Color(224, 224, 224, m_color.a); // 0xE0E0E0U
		int textWidth = font.width(getMessage()) * 2;
		font.drawScalableShadow(getMessage(), m_xPos + (m_width - textWidth) / 2, m_yPos + (m_height - 16) / 2, textColor);
	}
	else
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
}
