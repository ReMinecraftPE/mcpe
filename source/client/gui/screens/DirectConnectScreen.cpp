/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "DirectConnectScreen.hpp"
#include "ProgressScreen.hpp"
#include "JoinGameScreen.hpp"
#include "client/common/Util.hpp"

DirectConnectScreen::DirectConnectScreen() :
	m_textAddress(1, 0, 0, 0, 0, "Server address"),
	m_btnJoin(2, "Join Game"),
	m_btnQuit(3, "Cancel")
{}

void DirectConnectScreen::init()
{
	const int CONTAINER_PADDING = 5;
	const int BUTTON_CONTAINER_PADDING = 5;
	const int BUTTON_CONTAINER_WIDTH = 200;

	m_textAddress.m_width = 200;
	m_textAddress.m_height = 20;

	m_btnJoin.m_width = (BUTTON_CONTAINER_WIDTH / 2) - (BUTTON_CONTAINER_PADDING / 2);
	m_btnJoin.m_height = 20;

	m_btnQuit.m_width = (BUTTON_CONTAINER_WIDTH / 2) - (BUTTON_CONTAINER_PADDING / 2);
	m_btnQuit.m_height = 20;

	m_textAddress.m_xPos = (m_width / 2) - (m_textAddress.m_width / 2);
	m_textAddress.m_yPos = (m_height / 2) - m_btnJoin.m_height - CONTAINER_PADDING;

	m_btnJoin.m_xPos = (m_width / 2) - (m_btnJoin.m_width) - (BUTTON_CONTAINER_PADDING / 2);
	m_btnJoin.m_yPos = (m_height / 2) + CONTAINER_PADDING + m_textAddress.m_height;

	m_btnQuit.m_xPos = (m_width / 2) + (BUTTON_CONTAINER_PADDING / 2);
	m_btnQuit.m_yPos = (m_height / 2) + CONTAINER_PADDING + m_textAddress.m_height;

	m_textInputs.push_back(&m_textAddress);
	m_buttons.push_back(&m_btnQuit);
	m_buttons.push_back(&m_btnJoin);

	m_textAddress.init(m_pFont);
}

void DirectConnectScreen::render(int x, int y, float f)
{
	renderBackground();
	Screen::render(x, y, f);

	drawCenteredString(m_pFont, "Direct Connect", m_width / 2, 30, 0xFFFFFF);
	drawString(m_pFont, "Server address", m_textAddress.m_xPos, m_textAddress.m_yPos - 10, 0xDDDDDD);
}

void DirectConnectScreen::buttonClicked(Button* pButton)
{
	if (pButton->m_buttonId == m_btnJoin.m_buttonId)
	{
		if (!m_textAddress.m_text.empty())
		{
			PingedCompatibleServer newPgs;
			RakNet::SystemAddress sysAddress;

			sysAddress.FromString(m_textAddress.m_text.c_str());
			newPgs.m_address = sysAddress;

			m_pMinecraft->joinMultiplayer(newPgs);
			m_pMinecraft->setScreen(new ProgressScreen);

			m_btnJoin.m_bEnabled = false;
			m_textAddress.m_bEnabled = false;
		}
	}
	else if (pButton->m_buttonId == m_btnQuit.m_buttonId)
	{
		m_pMinecraft->setScreen(new JoinGameScreen);
	}
}
