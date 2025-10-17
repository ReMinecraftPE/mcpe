/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "PauseScreen.hpp"
#include "OptionsScreen.hpp"
#include "server/ServerSideNetworkHandler.hpp"

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
	bool bAddVisibleButton = m_pMinecraft->m_pRakNetInstance && m_pMinecraft->m_pRakNetInstance->m_bIsHost;
	
	int nButtons = 2;

	if (bAddVisibleButton)
		nButtons++;

#ifdef ENH_ADD_OPTIONS_PAUSE
	nButtons++;
#endif

	int currY = 48, inc = 32;

	bool cramped = m_height < currY + inc * nButtons + 10; // also add some padding
	if (cramped)
		inc = 25;

	m_btnQuit.m_width = 160;
	m_btnBack.m_width = 160;
	m_btnVisible.m_width = 160;
	m_btnQuitAndCopy.m_width = 160;

	m_btnBack.m_yPos = currY; currY += inc;
	m_btnQuit.m_yPos = currY; currY += inc;
	m_btnBack.m_xPos = (m_width - 160) / 2;
	m_btnQuit.m_xPos = (m_width - 160) / 2;
	m_btnVisible.m_xPos = (m_width - 160) / 2;
	m_btnQuitAndCopy.m_xPos = (m_width - 160) / 2;

	m_btnVisible.m_yPos =
	m_btnQuitAndCopy.m_yPos = currY;

#ifdef ENH_ADD_OPTIONS_PAUSE
	// TODO: when visible or quit&copy are on, lower this
	m_btnOptions.m_width = 160;
	m_btnOptions.m_yPos = currY;
	m_btnOptions.m_xPos = m_btnBack.m_xPos;
#endif
	currY += inc;

	// add the buttons to the screen:
	m_buttons.push_back(&m_btnBack);

#ifdef ENH_ADD_OPTIONS_PAUSE
	m_buttons.push_back(&m_btnOptions);
#endif

	if (bAddVisibleButton)
	{
		updateServerVisibilityText();
		m_buttons.push_back(&m_btnVisible);
#ifdef ENH_ADD_OPTIONS_PAUSE
		m_btnOptions.m_yPos += inc;
#endif
	}

	m_buttons.push_back(&m_btnQuit);
	
	//m_buttons.push_back(&m_btnQuitAndCopy);

#ifdef ENH_ADD_OPTIONS_PAUSE
	//swap the options and quit buttons around (??)
	std::swap(m_btnOptions.m_yPos, m_btnQuit.m_yPos);
#endif

	for (int i = 0; i < int(m_buttons.size()); i++)
		m_buttonTabList.push_back(m_buttons[i]);

#ifdef __EMSCRIPTEN__
	m_btnVisible.m_bEnabled = false;
#endif
}

void PauseScreen::updateServerVisibilityText()
{
	if (!m_pMinecraft->m_pRakNetInstance) return;
	if (!m_pMinecraft->m_pRakNetInstance->m_bIsHost) return;

	ServerSideNetworkHandler* pSSNH = (ServerSideNetworkHandler*)m_pMinecraft->m_pNetEventCallback;

	if (pSSNH->m_bAllowIncoming)
		m_btnVisible.m_text = "Server is visible";
	else
		m_btnVisible.m_text = "Server is invisible";
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
	if (pButton->m_buttonId == m_btnBack.m_buttonId)
		m_pMinecraft->resumeGame();

	if (pButton->m_buttonId == m_btnQuit.m_buttonId)
		m_pMinecraft->leaveGame(false);

	if (pButton->m_buttonId == m_btnQuitAndCopy.m_buttonId)
		m_pMinecraft->leaveGame(true);

	if (pButton->m_buttonId == m_btnVisible.m_buttonId)
	{
		if (m_pMinecraft->m_pRakNetInstance && m_pMinecraft->m_pRakNetInstance->m_bIsHost)
		{
			ServerSideNetworkHandler* pSSNH = (ServerSideNetworkHandler*)m_pMinecraft->m_pNetEventCallback;
			pSSNH->allowIncomingConnections(!pSSNH->m_bAllowIncoming);

			updateServerVisibilityText();
		}
	}

#ifdef ENH_ADD_OPTIONS_PAUSE
	if (pButton->m_buttonId == m_btnOptions.m_buttonId)
		m_pMinecraft->setScreen(new OptionsScreen);
#endif
}
