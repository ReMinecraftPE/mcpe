/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include <fstream>

#include "client/app/Minecraft.hpp"
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

	std::istringstream credits(m_pMinecraft->m_pPlatform->readAssetFileStr("credits.txt", false));
	if (!credits) {
		drawCenteredString(*m_pMinecraft->m_pFont, "Failed to load credits.txt", m_width / 2, 67, 0xFFFFFFFF);
		return;
	}

	int height = 28;
	std::string line;
	while (std::getline(credits, line)) {
		drawCenteredString(*m_pMinecraft->m_pFont, line, m_width / 2, height, 0xFFFFFFFF);
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
