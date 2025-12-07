/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "DisconnectionScreen.hpp"
#include "StartMenuScreen.hpp"

DisconnectionScreen::DisconnectionScreen(const std::string& text) :
	m_text(text),
	m_btnOK    (0, 0, 0, "OK")
{
}

void DisconnectionScreen::buttonClicked(Button* pButton)
{
	m_pMinecraft->setScreen(new StartMenuScreen);
}

void DisconnectionScreen::init()
{
	m_btnOK.m_width = 128;

	m_btnOK.m_xPos = (m_width / 2) - (m_btnOK.m_width / 2);
	m_btnOK.m_yPos = m_height / 2;

	m_buttons.push_back(&m_btnOK);
	m_buttonTabList.push_back(&m_btnOK);
}

void DisconnectionScreen::render(int mouseX, int mouseY, float f)
{
	renderBackground();
	drawCenteredString(*m_pFont, m_text, m_width / 2, m_height / 2 - 32, 0xFFFFFF);
	Screen::render(mouseX, mouseY, f);
}


