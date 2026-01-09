/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include <cstring>

#include "CreditsScreen.hpp"
#include "DeleteWorldScreen.hpp"
#include "CreateWorldScreen.hpp"
#include "StartMenuScreen.hpp"
#include "ProgressScreen.hpp"
#include "common/Util.hpp"

CreditsScreen::CreditsScreen() :
	m_btnBack(3, "Back"),
	m_pDarkBackground(nullptr)
{
}

CreditsScreen::~CreditsScreen()
{
    SAFE_DELETE(m_pDarkBackground);
}

void CreditsScreen::init()
{
	SAFE_DELETE(m_pDarkBackground);

	m_pDarkBackground = new WorldSelectionList(m_pMinecraft, m_width, m_height);
	m_pDarkBackground->commit();

	m_btnBack.m_yPos   = m_height - 28;
	m_btnBack.m_width  = 84;
	m_btnBack.m_height = 24;

	m_btnBack.m_xPos = m_width / 2 - m_btnBack.m_width / 2;

	m_buttons.push_back(&m_btnBack);

	m_buttonTabList.push_back(&m_btnBack);
}

bool CreditsScreen::isInGameScreen()
{
	return true;
}

void CreditsScreen::keyPressed(int code)
{
	Screen::keyPressed(code);
}

void CreditsScreen::tick()
{
	m_pDarkBackground->tick();
}

void CreditsScreen::render(int mouseX, int mouseY, float f)
{
	renderBackground();
	m_pDarkBackground->render(0, 0, f);

	Screen::render(mouseX, mouseY, f);

	drawCenteredString(*m_pMinecraft->m_pFont, "Credits", m_width / 2, 8, 0xFFFFFFFF);

	const char *credits =
		"Actually important people:\0"
		"Un1q32\0"
		"Some other guys who helped out too I guess:\0"
		"iProgramCPP\0"
		"BrentDaMage\0"
		"TheBrokenRail\0"
		"Vimdo\0"
		"Wilylcaro\0"
		"and all the other remcpe contributors\0";

	int height = 67;
	while (*credits != '\0') {
		drawCenteredString(*m_pMinecraft->m_pFont, credits, m_width / 2, height, 0xFFFFFFFF);
		credits += strlen(credits) + 1;
		height += 10;
	}
}

bool CreditsScreen::handleBackEvent(bool b)
{
	if (b)
		return true;

	m_pMinecraft->setScreen(new StartMenuScreen);
	return true;
}

void CreditsScreen::buttonClicked(Button* pButton)
{
	if (pButton->m_buttonId == m_btnBack.m_buttonId)
		m_pMinecraft->setScreen(new StartMenuScreen);
}
