/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "Button.hpp"

Button::Button(int a2, int xPos, int yPos, int btnWidth, int btnHeight, const std::string& text)
{
	field_30 = a2;
	m_xPos = xPos;
	m_yPos = yPos;
	field_18 = text;
	m_width  = btnWidth;
	m_height = btnHeight;
}

Button::Button(int a2, int xPos, int yPos, const std::string& text)
{
	field_30 = a2;
	m_xPos = xPos;
	m_yPos = yPos;
	field_18 = text;
	m_width  = 200;
	m_height = 24;
}

Button::Button(int a2, const std::string& text)
{
	field_30 = a2;
	field_18 = text;
	m_width  = 200;
	m_height = 24;
}

bool Button::clicked(Minecraft* pMinecraft, int xPos, int yPos)
{
	if (!m_bEnabled) return false;
	if (xPos < m_xPos) return false;
	if (yPos < m_yPos) return false;
	if (xPos >= m_xPos + m_width) return false;
	if (yPos >= m_yPos + m_height) return false;

	return true;
}

int Button::getYImage(bool bHovered)
{
	if (!m_bEnabled) return 0;
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
	if (!m_bVisible) return;

#ifdef ENH_HIGHLIGHT_BY_HOVER
	field_36 = clicked(pMinecraft, xPos, yPos);
#endif

	Font* pFont = pMinecraft->m_pFont;
	Textures* pTexs = pMinecraft->m_pTextures;

	pTexs->loadAndBindTexture("gui/gui.png");

	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	int iYPos = 20 * getYImage(field_36) + 46;

	blit(m_xPos, m_yPos, 0, iYPos, m_width / 2, m_height, 0, 20);
	blit(m_xPos + m_width / 2, m_yPos, 200 - m_width / 2, iYPos, m_width / 2, m_height, 0, 20);

	renderBg(pMinecraft, xPos, yPos);

	int textColor;
	if (!m_bEnabled)
		textColor = int(0xFFA0A0A0U);
	else if (field_36)
		textColor = int(0xFFFFA0U);
	else
		textColor = int(0xE0E0E0U);

	drawCenteredString(pFont, field_18, m_xPos + m_width / 2, m_yPos + (m_height - 8) / 2, textColor);
}
