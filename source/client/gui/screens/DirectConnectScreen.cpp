/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "DirectConnectScreen.hpp"
#include "ProgressScreen.hpp"
#include "JoinGameScreen.hpp"
#include "common/Util.hpp"

DirectConnectScreen::DirectConnectScreen() :
	m_textAddress(this, 1, 0, 0, 0, 0, "Server address"),
	m_btnQuit(3, "Cancel"),
	m_btnJoin(2, "Connect")
{}

void DirectConnectScreen::init()
{
	m_textAddress.m_width = 200;
	m_textAddress.m_height = 20;

	m_btnJoin.m_width = 200;
	m_btnJoin.m_height = 20;

	m_btnQuit.m_width = 200;
	m_btnQuit.m_height = 20;

	m_textAddress.m_xPos = (m_width / 2) - (m_textAddress.m_width / 2);
	m_textAddress.m_yPos = m_height / 2 - 20;

	m_btnJoin.m_xPos = (m_width / 2) - (m_btnJoin.m_width / 2);
	m_btnJoin.m_yPos = m_height - 55;

	m_btnQuit.m_xPos = (m_width / 2) - (m_btnQuit.m_width / 2);
	m_btnQuit.m_yPos = m_height - 30;

	m_textInputs.push_back(&m_textAddress);
	m_buttons.push_back(&m_btnQuit);
	m_buttons.push_back(&m_btnJoin);

	m_textAddress.init(m_pFont);
}

void DirectConnectScreen::render(int x, int y, float f)
{
	renderBackground();
	Screen::render(x, y, f);

	drawCenteredString(m_pFont, "Play Multiplayer", m_width / 2, 30, 0xFFFFFF);
	drawCenteredString(m_pFont, "Enter the IP address of a server to connect to it:", m_width / 2, m_textAddress.m_yPos - 20, 0x999999);
}

void DirectConnectScreen::buttonClicked(Button* pButton)
{
	if (pButton->m_buttonId == m_btnJoin.m_buttonId)
	{
		if (m_textAddress.m_text.empty())
			return;

		PingedCompatibleServer newPgs;
		RakNet::SystemAddress sysAddress;

		if (!sysAddress.FromString(m_textAddress.m_text.c_str()))
			return;

		if (sysAddress.GetPort() == 0)
			sysAddress.SetPortHostOrder(C_DEFAULT_PORT);

		newPgs.m_address = sysAddress;

		m_pMinecraft->joinMultiplayer(newPgs);
		m_pMinecraft->setScreen(new ProgressScreen);

		m_btnJoin.m_bEnabled = false;
		m_textAddress.m_bEnabled = false;
	}
	else if (pButton->m_buttonId == m_btnQuit.m_buttonId)
	{
		m_pMinecraft->setScreen(new JoinGameScreen);
	}
}
