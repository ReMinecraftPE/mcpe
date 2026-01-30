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
	m_width = 0;
	m_height = 0;
	m_xPos = 0;
	m_yPos = 0;
	m_text = "";
	m_color = Color::WHITE;
	m_uiProfile = UI_POCKET;

#ifndef ORIGINAL_CODE
	m_lastX = 0;
	m_lastY = 0;
#endif
}

Button::Button(int buttonId, int xPos, int yPos, int btnWidth, int btnHeight, const std::string& text)
{
	_init();

	m_buttonId = buttonId;
	m_xPos = xPos;
	m_yPos = yPos;
	m_text = text;
	m_width  = btnWidth;
	m_height = btnHeight;
}

Button::Button(int buttonId, int xPos, int yPos, const std::string& text)
{
	_init();

	m_buttonId = buttonId;
	m_xPos = xPos;
	m_yPos = yPos;
	m_text = text;
	m_width  = 200;
	m_height = 24;
}

Button::Button(int buttonId, const std::string& text)
{
	_init();

	m_buttonId = buttonId;
	m_text = text;
	m_width  = 200;
	m_height = 24;
}

bool Button::clicked(Minecraft* pMinecraft, int xPos, int yPos)
{
	if (!isEnabled()) return false;
	if (xPos < m_xPos) return false;
	if (yPos < m_yPos) return false;
	if (xPos >= m_xPos + m_width) return false;
	if (yPos >= m_yPos + m_height) return false;

	return true;
}

int Button::getYImage(bool bHovered)
{
	if (!isEnabled()) return 0;
	if (bHovered) return 2;
	return 1;
}

void Button::released(int xPos, int yPos)
{

}

void Button::renderBg(Minecraft*, int, int)
{

}

void Button::render(Minecraft* pMinecraft, int xPos, int yPos)
{
	if (!isVisible()) return;

	if (!pMinecraft->m_pScreen->doElementTabbing())
		setSelected(clicked(pMinecraft, xPos, yPos));

	if (m_color.a == 0.0f)
		return;

	Font& font = *pMinecraft->m_pFont;
	Textures& texs = *pMinecraft->m_pTextures;

	texs.loadAndBindTexture("gui/gui.png");

	currentShaderColor = m_color;

	if (m_uiProfile == UI_LEGACY)
	{
		texs.setSmoothing(true);
		blitTexture(texs, isSelected() ? "gui/highlighted_button.png" : "gui/button.png", m_xPos, m_yPos, 0, 0, m_width, m_height);
		texs.setSmoothing(false);
	}
	else
	{
		int iYPos = 20 * getYImage(isSelected()) + 46;

		blit(m_xPos, m_yPos, 0, iYPos, m_width / 2, m_height, 0, 20, &m_materials.ui_textured_and_glcolor);
		blit(m_xPos + m_width / 2, m_yPos, 200 - m_width / 2, iYPos, m_width / 2, m_height, 0, 20, &m_materials.ui_textured_and_glcolor);
	}

	renderBg(pMinecraft, xPos, yPos);

	Color textColor;
	if (!isEnabled())
		textColor = Color(160, 160, 160, m_color.a); // 0xFFA0A0A0
	else if (isSelected())
		textColor = Color(255, 255, 160, m_color.a); // 0xFFFFA0U
	else
		textColor = Color(224, 224, 224, m_color.a); // 0xE0E0E0U

	if (m_uiProfile == UI_LEGACY)
	{
		int textWidth = font.width(m_text) * 2;
		font.drawLegacyShadow(m_text, m_xPos + (m_width - textWidth) / 2, m_yPos + (m_height - 16) / 2, textColor);
	}
	else
	{
		drawCenteredString(font, m_text, m_xPos + m_width / 2, m_yPos + (m_height - 8) / 2, textColor);
	}
}
