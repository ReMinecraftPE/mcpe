/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "InvalidLicenseScreen.hpp"

InvalidLicenseScreen::InvalidLicenseScreen(int error, bool bHasQuitButton) :
	m_error(error),
	m_btnOk (1, "Ok"),
	m_btnBuy(2, "Buy"),
	m_bHasQuitButton(bHasQuitButton)
{
	if (bHasQuitButton)
		m_btnOk.field_18 = "Quit";
}

void InvalidLicenseScreen::buttonClicked(Button* pButton)
{
	if (pButton->field_30 == m_btnOk.field_30)
	{
		m_pMinecraft->quit();
	}

	if (pButton->field_30 == m_btnBuy.field_30)
	{
		m_pMinecraft->platform()->buyGame();
	}
}

void InvalidLicenseScreen::init()
{
	field_E4 = m_height / 3;
	if (m_bHasQuitButton)
		field_E4 -= 24;

	if (m_error > 1)
	{
		char str[20] = { 0 };
		sprintf(str, "%d", m_error);
		m_textLine1 = "License verification failed (error " + std::string(str) + ")";
		m_textLine2 = "Try again later.";
	}

	m_btnOk.m_xPos = m_btnBuy.m_xPos = (m_width - m_btnOk.m_width) / 2;

	m_btnOk.m_yPos  = field_E4 + 60;
	m_btnBuy.m_yPos = field_E4 + 88;

	m_buttons.push_back(&m_btnOk);
	m_buttons.push_back(&m_btnBuy);
	m_buttonTabList.push_back(&m_btnOk);
	m_buttonTabList.push_back(&m_btnBuy);
}

void InvalidLicenseScreen::tick()
{
}

void InvalidLicenseScreen::render(int mouseX, int mouseY, float f)
{
	renderDirtBackground(0);
	drawCenteredString(m_pMinecraft->m_pFont, m_textLine1, m_width / 2, field_E4, 0xFFFFFF);
	drawCenteredString(m_pMinecraft->m_pFont, m_textLine2, m_width / 2, field_E4 + 24, 0xFFFFFF);
	Screen::render(mouseX, mouseY, f);
}
