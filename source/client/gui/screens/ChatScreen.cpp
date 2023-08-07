/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "ChatScreen.hpp"

// @NOTE: This is unused.

ChatScreen::ChatScreen(bool slash) : m_textChat(1, 0, 0), m_btnSend(2, 0, 0, "Send")
{
	if (slash)
		m_textChat.m_text = "/";
}

void ChatScreen::buttonClicked(Button* pButton)
{
	if (pButton->m_buttonId == m_btnSend.m_buttonId)
		sendMessageAndExit();
}

void ChatScreen::init()
{
	m_btnSend.m_height = 20;
	m_btnSend.m_width = 40;
	m_textChat.m_xPos = 0;
	m_textChat.m_yPos = m_height - 20;
	m_textChat.m_width = m_width - m_btnSend.m_width;
	m_textChat.m_height = 20;
	m_btnSend.m_yPos = m_height - 20;
	m_btnSend.m_xPos = m_textChat.m_xPos + m_textChat.m_width;
	
	// set focus directly on the chat text box
	m_textChat.init(m_pFont);
	m_textChat.setFocused(true);

	m_buttons.push_back(&m_btnSend);
	m_textInputs.push_back(&m_textChat);
}

void ChatScreen::removed()
{
	// Now let them be rendered.
	m_pMinecraft->m_gui.m_bRenderMessages = true;
}

void ChatScreen::render(int mouseX, int mouseY, float f)
{
	renderBackground();

	// override the default behavior of rendering chat messages
	m_pMinecraft->m_gui.m_bRenderMessages = false;
	m_pMinecraft->m_gui.renderMessages(true);

	Screen::render(mouseX, mouseY, f);
}

void ChatScreen::keyPressed(int keyCode)
{
	if (keyCode == AKEYCODE_ENTER)
		sendMessageAndExit();

	Screen::keyPressed(keyCode);
}

void ChatScreen::sendMessageAndExit()
{
	m_pMinecraft->sendMessage(m_textChat.m_text);
	
	m_pMinecraft->setScreen(nullptr);
}
