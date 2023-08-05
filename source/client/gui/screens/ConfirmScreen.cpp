/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "ConfirmScreen.hpp"

ConfirmScreen::ConfirmScreen(Screen* pScreen, const std::string& line1, const std::string& line2, int x) :
	m_btnOK    (0, 0, 0, "Ok"),
	m_btnCancel(1, 0, 0, "Cancel"),
	m_textLine1(line1),
	m_textLine2(line2),
	m_pScreen(pScreen),
	field_40(x)
{
}

ConfirmScreen::ConfirmScreen(Screen* pScreen, const std::string& line1, const std::string& line2, const std::string& ok, const std::string& cancel, int x) :
	m_btnOK    (0, 0, 0, ok),
	m_btnCancel(1, 0, 0, cancel),
	m_textLine1(line1),
	m_textLine2(line2),
	m_pScreen(pScreen),
	field_40(x)
{
}

// @NOTE: potential memory leak if pScreen is set and not destroyed!

void ConfirmScreen::buttonClicked(Button* pButton)
{
	postResult(pButton->field_30 == 0);
}

bool ConfirmScreen::handleBackEvent(bool b)
{
	if (!b)
		postResult(false);

	return true;
}

void ConfirmScreen::init()
{
	m_btnOK.m_xPos = m_width / 2 - 4 - 120;
	m_btnCancel.m_xPos = m_width / 2 + 4;

	m_btnCancel.m_yPos = m_btnOK.m_yPos = m_height / 6 + 72;

	m_btnOK.m_width  = m_btnCancel.m_width = 120;
	m_btnOK.m_height = m_btnCancel.m_height = 24;

	m_buttons.push_back(&m_btnOK);
	m_buttons.push_back(&m_btnCancel);
	m_buttonTabList.push_back(&m_btnOK);
	m_buttonTabList.push_back(&m_btnCancel);
}

void ConfirmScreen::render(int mouseX, int mouseY, float f)
{
	renderBackground();
	drawCenteredString(m_pFont, m_textLine1, m_width / 2, 50, 0xFFFFFF);
	drawCenteredString(m_pFont, m_textLine2, m_width / 2, 70, 0xFFFFFF);
	Screen::render(mouseX, mouseY, f);
}

void ConfirmScreen::postResult(bool b)
{
	m_pScreen->confirmResult(b, field_40);
}


