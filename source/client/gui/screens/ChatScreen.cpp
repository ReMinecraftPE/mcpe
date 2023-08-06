/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "ChatScreen.hpp"

ChatScreen::ChatScreen() :
	m_btnSend(1, 0, 0, 0, 0, "Send"),
	m_textChat(2, 0, 0, 0, 0, "G'day!", "")
{
}

ChatScreen::ChatScreen() : m_textChat(1, 0, 0), m_btnSend(2, 0, 0, "Send")
{
}

void ChatScreen::sendMessage()
{
if (m_textChat.m_text.size() > 0)
		{
			#ifdef __EMSCRIPTEN__
			if (m_textChat.m_text[0] == '/')
			{
				if (m_textChat.m_text.substr(1) == "test")
				{
					m_pMinecraft->m_gui.addMessage("Test command invoked!");
				}
			}

			else
			{
				m_pMinecraft->m_gui.addMessage(m_pMinecraft->m_pLocalPlayer->m_name + ": " + m_textInput.m_text);
			}
			#else
			if (m_pMinecraft->m_pRakNetInstance->m_bIsHost)
			{
				if (m_textChat.m_text[0] == '/')
				{
					if (m_textChat.m_text.substr(1) == "test")
					{
						m_pMinecraft->m_pRakNetInstance->send(new MessagePacket("Test command invoked!"));
						m_pMinecraft->m_gui.addMessage("Test command invoked!");
					}
				}

				else
				{
					m_pMinecraft->m_pRakNetInstance->send(new MessagePacket(m_pMinecraft->m_pLocalPlayer->m_name + ": " + m_textChat.m_text));
					m_pMinecraft->m_gui.addMessage(m_pMinecraft->m_pLocalPlayer->m_name + ": " + m_textChat.m_text);
				}

			}
			else
			{
				m_pMinecraft->m_pRakNetInstance->send(new MessagePacket(m_textChat.m_text));
			}
			#endif

			m_textChat.m_text.clear();
		}
}

void ChatScreen::buttonClicked(Button* pButton)
{
	if (pButton->m_buttonId == m_btnSend.m_buttonId)
	{
		sendMessage();
	}
}

void ChatScreen::tick()
{
	m_chatLog.clear();
	m_chatLog.assign(m_pMinecraft->m_gui.m_guiMessages.begin(), m_pMinecraft->m_gui.m_guiMessages.end());
	m_chatLog.resize(6);
}

void ChatScreen::init()
{
	m_textChat.m_width  = m_width - 30;
	m_textChat.m_height = 25;
	m_textChat.m_xPos   = 0;
	m_textChat.m_yPos   = m_height - 25;

	m_btnSend.m_xPos     = m_textChat.m_width;

	m_btnSend.m_yPos     = m_height - 25;
	m_btnSend.m_width    = 30;
	m_btnSend.m_height   = 25;

	m_textInputs.push_back(&m_textChat);
	
	m_buttons.push_back(&m_btnSend);
	m_buttonTabList.push_back(&m_btnSend);

	m_textChat.init(m_pFont);
}

void ChatScreen::removed()
{
	m_pMinecraft->m_gui.m_bRenderMessages = true;
}

void ChatScreen::render(int mouseX, int mouseY, float f)
{
	Screen::render(mouseX, mouseY, f);
	renderBackground();
	
	glDisable(GL_DEPTH_TEST);

	fill(0, 0, m_width, m_height, 0x80000000);

	glEnable(GL_BLEND);
	
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	m_pMinecraft->m_pTextures->loadAndBindTexture("gui/gui.png");

	std::vector<GuiMessage> r;
	r.assign(m_chatLog.rbegin(), m_chatLog.rend());

	int i = 2;
	for (auto it : r) drawString(m_pFont, it.msg + "\n", m_textChat.m_xPos, (i++)*10, 0xDDDDDD);

	glEnable(GL_DEPTH_TEST);
	renderBackground();

	// override the default behavior of rendering chat messages
	m_pMinecraft->m_gui.m_bRenderMessages = false;
	m_pMinecraft->m_gui.renderMessages(true);

	Screen::render(mouseX, mouseY, f);
}

void ChatScreen::keyPressed(int keyCode)
{
	if (keyCode == AKEYCODE_ENTER)
	{
		sendMessage();
		m_pMinecraft->setScreen(nullptr);
	}
}