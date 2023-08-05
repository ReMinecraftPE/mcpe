/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "PauseScreen.hpp"
#include "OptionsScreen.hpp"
#include "client/network/ServerSideNetworkHandler.hpp"

PauseScreen::PauseScreen() :
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
	m_btnQuit.m_width = 160;
	m_btnBack.m_width = 160;
	m_btnVisible.m_width = 160;
	m_btnQuitAndCopy.m_width = 160;

	m_btnBack.m_yPos = 48;
	m_btnQuit.m_yPos = 80;
	m_btnBack.m_xPos = (m_width - 160) / 2;
	m_btnQuit.m_xPos = (m_width - 160) / 2;
	m_btnVisible.m_xPos = (m_width - 160) / 2;
	m_btnQuitAndCopy.m_xPos = (m_width - 160) / 2;

	m_btnVisible.m_yPos = 112;
	m_btnQuitAndCopy.m_yPos = 112;

#ifdef ENH_ADD_OPTIONS_PAUSE
	// TODO: when visible or quit&copy are on, lower this
	m_btnOptions.m_width = 160;
	m_btnOptions.m_yPos = 112;
	m_btnOptions.m_xPos = m_btnBack.m_xPos;
#endif

	// add the buttons to the screen:
	m_buttons.push_back(&m_btnBack);
	m_buttons.push_back(&m_btnQuit);

#ifdef ENH_ADD_OPTIONS_PAUSE
	m_buttons.push_back(&m_btnOptions);
#endif
	
	//m_buttons.push_back(&m_btnQuitAndCopy);

	if (m_pMinecraft->m_pRakNetInstance && m_pMinecraft->m_pRakNetInstance->m_bIsHost)
	{
		updateServerVisibilityText();
		m_buttons.push_back(&m_btnVisible);
#ifdef ENH_ADD_OPTIONS_PAUSE
		m_btnOptions.m_yPos += 32;
#endif
	}

	for (auto thing : m_buttons)
		m_buttonTabList.push_back(thing);
}

void PauseScreen::updateServerVisibilityText()
{
	if (!m_pMinecraft->m_pRakNetInstance) return;
	if (!m_pMinecraft->m_pRakNetInstance->m_bIsHost) return;

	ServerSideNetworkHandler* pSSNH = (ServerSideNetworkHandler*)m_pMinecraft->m_pNetEventCallback;

	if (pSSNH->m_bAllowIncoming)
		m_btnVisible.field_18 = "Server is visible";
	else
		m_btnVisible.field_18 = "Server is invisible";
}

void PauseScreen::tick()
{
	field_40++;
}

void PauseScreen::render(int a, int b, float c)
{
	renderBackground();

	drawCenteredString(m_pFont, "Game menu", m_width / 2, 24, 0xFFFFFF);
	Screen::render(a, b, c);
}

void PauseScreen::buttonClicked(Button* pButton)
{
	if (pButton->field_30 == m_btnBack.field_30)
		m_pMinecraft->setScreen(nullptr);

	if (pButton->field_30 == m_btnQuit.field_30)
		m_pMinecraft->leaveGame(false);

	if (pButton->field_30 == m_btnQuitAndCopy.field_30)
		m_pMinecraft->leaveGame(true);

	if (pButton->field_30 == m_btnVisible.field_30)
	{
		if (m_pMinecraft->m_pRakNetInstance && m_pMinecraft->m_pRakNetInstance->m_bIsHost)
		{
			ServerSideNetworkHandler* pSSNH = (ServerSideNetworkHandler*)m_pMinecraft->m_pNetEventCallback;
			pSSNH->allowIncomingConnections(!pSSNH->m_bAllowIncoming);

			updateServerVisibilityText();
		}
	}

#ifdef ENH_ADD_OPTIONS_PAUSE
	if (pButton->field_30 == m_btnOptions.field_30)
		m_pMinecraft->setScreen(new OptionsScreen);
#endif
}
