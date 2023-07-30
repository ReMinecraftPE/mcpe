/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "ChatScreen.hpp"

// @NOTE: This is unused.

void ChatScreen::buttonClicked(Button* pButton)
{
}

void ChatScreen::init()
{
	m_pMinecraft->platform()->showDialog(AppPlatform::DLG_CHAT);
	m_pMinecraft->platform()->createUserInput();
}

void ChatScreen::render(int mouseX, int mouseY, float f)
{
	int userInputStatus = m_pMinecraft->platform()->getUserInputStatus();
	if (userInputStatus < 0)
		return;

	if (userInputStatus == 1)
	{
		std::vector<std::string> userInput = m_pMinecraft->platform()->getUserInput();
		if (userInput.size() >= 1)
		{
			// @NOTE: No sending multiplayer chats. Weird
			m_pMinecraft->m_gui.addMessage(userInput[0]);
		}
	}

	m_pMinecraft->setScreen(nullptr);
}
