/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "ChatScreen.hpp"

// @NOTE: This is unused.

ChatScreen::ChatScreen(bool slash) : m_textChat(this, 0, 0), m_btnSend(0, 0, "Send")
{
	if (slash)
		m_textChat.setTextboxText("/");
}

void ChatScreen::_buttonClicked(Button* pButton)
{
	if (pButton->getId() == m_btnSend.getId())
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

	_addElement(m_textChat);
	_addElement(m_btnSend);
}

void ChatScreen::removed()
{
	// Now let them be rendered.
	m_pMinecraft->m_pGui->m_bRenderMessages = true;
}

void ChatScreen::render(float f)
{
	renderBackground();

	// override the default behavior of rendering chat messages
	m_pMinecraft->m_pGui->m_bRenderMessages = false;
	m_pMinecraft->m_pGui->renderMessages(true);

	Screen::render(f);
}

void ChatScreen::keyPressed(int keyCode)
{
	if (m_pMinecraft->getOptions()->isKey(KM_MENU_OK, keyCode))
		sendMessageAndExit();

	Screen::keyPressed(keyCode);
}

void ChatScreen::sendMessageAndExit()
{
	m_pMinecraft->sendMessage(m_textChat.getText());
	
	m_pMinecraft->setScreen(nullptr);
}
