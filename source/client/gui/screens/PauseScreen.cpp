/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "PauseScreen.hpp"
#include "OptionsScreen.hpp"
#include "server/ServerSideNetworkHandler.hpp"
#include "client/renderer/LogoRenderer.hpp"

PauseScreen::PauseScreen() :
	//m_oPos(0),
	field_40(0),
	m_btnBack(1, "Back to game"),
	m_btnQuit(2, "Quit to title"),
	m_btnQuitAndCopy(3, "Quit and copy map"),
	m_btnVisible(4, "")
#ifdef ENH_ADD_OPTIONS_PAUSE
	, m_btnOptions(999, "Options")
#endif
{
}

void PauseScreen::init()
{
	m_uiTheme = m_pMinecraft->getOptions()->m_uiTheme;

	bool consoleUI = m_uiTheme == UI_CONSOLE;

	bool bAddVisibleButton = m_pMinecraft->m_pRakNetInstance && m_pMinecraft->m_pRakNetInstance->m_bIsHost;

	std::vector<Button*> layoutButtons;

	layoutButtons.push_back(&m_btnBack);
#ifdef ENH_ADD_OPTIONS_PAUSE
	layoutButtons.push_back(&m_btnOptions);
#endif
	if (bAddVisibleButton)
	{
		updateServerVisibilityText();
		layoutButtons.push_back(&m_btnVisible);
	}
	layoutButtons.push_back(&m_btnQuit);

	int buttonsWidth;
	int buttonsHeight;
	int y;
	int ySpacing;

	if (consoleUI)
	{
		buttonsWidth = 400;
		buttonsHeight = 40;
		y = m_height / 3 + 10;
		ySpacing = 50;
	}
	else
	{
		buttonsWidth = 160;
		buttonsHeight = 25;
		y = 48;
		ySpacing = 32;
		bool cramped = m_height < y + ySpacing * int(layoutButtons.size()) + 10; // also add some padding
		if (cramped)
			ySpacing = 25;
	}

	for (size_t i = 0; i < layoutButtons.size(); ++i)
	{
		Button* button = layoutButtons[i];
		button->m_width = buttonsWidth;
		button->m_height = buttonsHeight;
		button->m_xPos = (m_width - button->m_width) / 2;
		button->m_yPos = y + ySpacing * i;
		_addElement(*button);
	}

#ifndef FEATURE_NETWORKING
	m_btnVisible.setEnabled(false);
#endif
}

void PauseScreen::updateServerVisibilityText()
{
	if (!m_pMinecraft->m_pRakNetInstance) return;
	if (!m_pMinecraft->m_pRakNetInstance->m_bIsHost) return;

	ServerSideNetworkHandler* pSSNH = (ServerSideNetworkHandler*)m_pMinecraft->m_pNetEventCallback;

	if (pSSNH->m_bAllowIncoming)
		m_btnVisible.setMessage("Server is visible");
	else
		m_btnVisible.setMessage("Server is invisible");
}

void PauseScreen::tick()
{
	field_40++;
}

void PauseScreen::render(float f)
{
	renderBackground();

	if (m_uiTheme == UI_CONSOLE)
	{
		LogoRenderer::singleton().render(f);
	}
	else
	{
		drawCenteredString(*m_pFont, "Game menu", m_width / 2, 24, 0xFFFFFF);
	}

	Screen::render(f);
}

void PauseScreen::_buttonClicked(Button* pButton)
{
	if (pButton->getId() == m_btnBack.getId())
		m_pMinecraft->resumeGame();

	if (pButton->getId() == m_btnQuit.getId())
		m_pMinecraft->leaveGame(false);

	if (pButton->getId() == m_btnQuitAndCopy.getId())
		m_pMinecraft->leaveGame(true);

	if (pButton->getId() == m_btnVisible.getId())
	{
		if (m_pMinecraft->m_pRakNetInstance && m_pMinecraft->m_pRakNetInstance->m_bIsHost)
		{
			ServerSideNetworkHandler* pSSNH = (ServerSideNetworkHandler*)m_pMinecraft->m_pNetEventCallback;
			pSSNH->allowIncomingConnections(!pSSNH->m_bAllowIncoming);

			updateServerVisibilityText();
		}
	}

#ifdef ENH_ADD_OPTIONS_PAUSE
	if (pButton->getId() == m_btnOptions.getId())
		m_pMinecraft->setScreen(new OptionsScreen(this));
#endif
}
