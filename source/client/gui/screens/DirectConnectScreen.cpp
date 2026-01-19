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
	m_textAddress(this, 1, 0, 0, 0),
	m_btnCancel(3, "Cancel"),
	m_btnJoin(2, "Join Server")
{}

void DirectConnectScreen::_buttonClicked(Button* pButton)
{
	if (pButton->m_buttonId == m_btnJoin.m_buttonId)
	{
		if (m_textAddress.getText().empty())
			return;

		PingedCompatibleServer newPgs;
		RakNet::SystemAddress sysAddress;

		if (!sysAddress.FromString(m_textAddress.getText().c_str()))
			return;

		if (sysAddress.GetPort() == 0)
			sysAddress.SetPortHostOrder(C_DEFAULT_PORT);

		newPgs.m_address = sysAddress;

		m_pMinecraft->joinMultiplayer(newPgs);
		m_pMinecraft->setScreen(new ProgressScreen);

		m_btnJoin.setEnabled(false);
		m_textAddress.setEnabled(false);
	}
	else if (pButton->m_buttonId == m_btnCancel.m_buttonId)
	{
		DirectConnectScreen::handleBackEvent(false);
	}
}

void DirectConnectScreen::init()
{
	m_textAddress.m_width = 200;
	m_textAddress.m_height = 20;

	m_btnJoin.m_width = 200;
	m_btnJoin.m_height = 20;

	m_btnCancel.m_width = 200;
	m_btnCancel.m_height = 20;

	m_textAddress.m_xPos = (m_width / 2) - (m_textAddress.m_width / 2);
	m_textAddress.m_yPos = m_height / 2 - 20;

	m_btnJoin.m_xPos = (m_width / 2) - (m_btnJoin.m_width / 2);
	m_btnJoin.m_yPos = m_height - 55;
	m_btnJoin.setEnabled(false);

	m_btnCancel.m_xPos = (m_width / 2) - (m_btnCancel.m_width / 2);
	m_btnCancel.m_yPos = m_height - 30;

	m_textInputs.push_back(&m_textAddress);
	_addElement(m_btnJoin);
	_addElement(m_btnCancel);

	m_textAddress.init(m_pFont);
}

void DirectConnectScreen::render(float f)
{
	renderBackground();
	Screen::render(f);

	drawCenteredString(*m_pFont, "Direct Connect", m_width / 2, 30, 0xFFFFFF);
	drawString(*m_pFont, "Server Address", m_textAddress.m_xPos, m_textAddress.m_yPos - 15, 0x999999);
}

bool DirectConnectScreen::handleBackEvent(bool b)
{
	if (!b)
	{
		m_pMinecraft->setScreen(new JoinGameScreen);
	}

	return true;
}

void DirectConnectScreen::onTextBoxUpdated(int id)
{
	if (id == m_textAddress.getKey())
	{
		if (m_textAddress.getText().empty())
			m_btnJoin.setEnabled(false);
		else
			m_btnJoin.setEnabled(true);
	}
}
