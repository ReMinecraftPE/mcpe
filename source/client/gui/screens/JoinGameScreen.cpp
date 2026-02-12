/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "JoinGameScreen.hpp"
#include "DirectConnectScreen.hpp"

JoinGameScreen::JoinGameScreen() :
	m_btnJoin("Join Game"),
	m_btnDirectConnect("Direct Connect"),
	m_btnBack("Back"),
	m_pAvailableGamesList(nullptr)
{
}

JoinGameScreen::~JoinGameScreen()
{
	SAFE_DELETE(m_pAvailableGamesList);
}

void JoinGameScreen::_buttonClicked(Button* pButton)
{
	if (pButton->getId() == m_btnJoin.getId())
	{
		if (isIndexValid(m_pAvailableGamesList->m_selectedIndex))
		{
			m_pMinecraft->joinMultiplayer(m_pAvailableGamesList->m_games[m_pAvailableGamesList->m_selectedIndex]);
			m_pMinecraft->getScreenChooser()->pushProgressScreen();

			m_btnJoin.setEnabled(false);
			m_btnDirectConnect.setEnabled(false);
			m_btnBack.setEnabled(false);
		}
	}

	if (pButton->getId() == m_btnDirectConnect.getId())
	{
		m_pMinecraft->setScreen(new DirectConnectScreen);
	}

	if (pButton->getId() == m_btnBack.getId())
	{
		m_pMinecraft->getScreenChooser()->pushStartScreen();
	}
}

bool JoinGameScreen::handleBackEvent(bool b)
{
	if (!b)
	{
		m_pMinecraft->cancelLocateMultiplayer();
		m_pMinecraft->getScreenChooser()->pushStartScreen();
	}

	return true;
}

void JoinGameScreen::init()
{
	const int BUTTON_WIDTH = 84;

	m_btnBack.m_yPos = m_btnJoin.m_yPos = m_btnDirectConnect.m_yPos = m_height - 27;
	m_btnBack.m_width = m_btnJoin.m_width = BUTTON_WIDTH;

	m_btnJoin.m_xPos = m_width / 2 - (BUTTON_WIDTH + (BUTTON_WIDTH / 2)) - 4;
	m_btnDirectConnect.m_xPos = (m_width / 2) - (BUTTON_WIDTH / 2);
	m_btnBack.m_xPos = m_width / 2 + 4 + (BUTTON_WIDTH / 2);

	m_btnDirectConnect.m_width = BUTTON_WIDTH;

	_addElement(m_btnJoin);
	_addElement(m_btnDirectConnect);
	_addElement(m_btnBack);
	
	if (m_pMinecraft->m_pRakNetInstance)
		m_pMinecraft->m_pRakNetInstance->clearServerList();

	m_pAvailableGamesList = new AvailableGamesList(m_pMinecraft, m_width, m_height, 24, m_height - 30, 28);
}

bool JoinGameScreen::isInGameScreen()
{
	return false;
}

void JoinGameScreen::render(float f)
{
	renderBackground();
	m_pAvailableGamesList->render(m_menuPointer, f);
	Screen::render(f);

	drawCenteredString(*m_pMinecraft->m_pFont, "Scanning for Games...", m_width / 2, 8, 0xFFFFFFFF);
}

void JoinGameScreen::tick()
{
	std::vector<PingedCompatibleServer> *serverList, serverListFiltered;
	serverList = m_pMinecraft->m_pRakNetInstance->getServerList();

	for (size_t i = 0; i < serverList->size(); i++)
	{
		const PingedCompatibleServer& pcs = (*serverList)[i];
		if (pcs.m_name.GetLength())
			serverListFiltered.push_back(pcs);
	}
	
	if (serverListFiltered.size() != m_pAvailableGamesList->m_games.size())
	{
		PingedCompatibleServer selectedItem;

		if (isIndexValid(m_pAvailableGamesList->m_selectedIndex))
		{
			selectedItem = m_pAvailableGamesList->m_games[m_pAvailableGamesList->m_selectedIndex];

			m_pAvailableGamesList->m_games = serverListFiltered;
			m_pAvailableGamesList->selectItem(-1, false);

			// relocate the new list item, if possible
			for (size_t i = 0; i < serverListFiltered.size(); i++)
			{
				if (serverListFiltered[i].m_address == selectedItem.m_address)
				{
					m_pAvailableGamesList->selectItem(i, false);
					break;
				}
			}
		}
		else
		{
			m_pAvailableGamesList->m_games = serverListFiltered;
			m_pAvailableGamesList->selectItem(-1, false);
		}
	}
	else if (!serverListFiltered.empty())
	{
		// Update the names of the items already in the available games list.
		// @BUG: What would happen if the filtered server list removes an IP and adds another all in the same tick?

		std::vector<PingedCompatibleServer>* pGames = &m_pAvailableGamesList->m_games;
		for (int i = int(pGames->size() - 1); i >= 0; i--)
		{
			size_t j = 0;
			for (; j < serverListFiltered.size(); j++)
			{
				if (serverListFiltered[j].m_address == (*pGames)[i].m_address)
					break;
			}

			if (j == serverListFiltered.size())
				continue;

			(*pGames)[i].m_name = serverListFiltered[j].m_name;
		}
	}

	m_btnJoin.setEnabled(isIndexValid(m_pAvailableGamesList->m_selectedIndex));
}

bool JoinGameScreen::isIndexValid(int idx)
{
	if (!m_pAvailableGamesList)
		return false;

	if (idx < 0)
		return false;
	if (idx >= m_pAvailableGamesList->getNumberOfItems())
		return false;

	return true;
}
