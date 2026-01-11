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

static std::vector<std::string> credits;

void CreditsScreen::render(int mouseX, int mouseY, float f)
{
	renderBackground();
	m_pDarkBackground->render(0, 0, f);

	Screen::render(mouseX, mouseY, f);

	drawCenteredString(*m_pMinecraft->m_pFont, "Credits", m_width / 2, 8, Color::WHITE);

	if (credits.empty()) {
		std::istringstream credits_stream(m_pMinecraft->m_pPlatform->readAssetFileStr("credits.txt", false));
		if (credits_stream.str().empty())
			credits_stream.str("Failed to load credits.txt");

		std::string line;
		while (std::getline(credits_stream, line))
			credits.push_back(line);
	}

	int lineheight = 10;
	int height = (m_height - (credits.size() * lineheight)) / 2 - 2;
	for (size_t i = 0; i < credits.size(); ++i) {
		drawCenteredString(*m_pMinecraft->m_pFont, credits[i], m_width / 2, height, Color::WHITE);
		height += lineheight;
	}
}

bool CreditsScreen::handleBackEvent(bool b)
{
	if (b)
		return true;

	std::vector<std::string>().swap(credits);
	m_pMinecraft->setScreen(new StartMenuScreen);
	return true;
}

void CreditsScreen::buttonClicked(Button* pButton)
{
	if (pButton->m_buttonId == m_btnBack.m_buttonId)
		m_pMinecraft->setScreen(new StartMenuScreen);
}
