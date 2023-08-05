/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "ChatScreen.hpp"
#include "StartMenuScreen.hpp"
#include <iostream>
#include <string>
#include <sstream>
ChatScreen::ChatScreen()
	: m_textInput(1, 0, 0, 0, 0, "yello!", ""),
	m_sendButton(2, 0, 0, "Send")
{
	
}



void ChatScreen::buttonClicked(Button* pButton)
{
	if (pButton->field_30 == m_sendButton.field_30)
	{
		if (m_textInput.m_text.size() >= 1)
		{
			if (m_pMinecraft->m_pRakNetInstance->m_bIsHost)
			{
				m_pMinecraft->m_pRakNetInstance->send(new MessagePacket(m_pMinecraft->m_pLocalPlayer->m_name + ": " + m_textInput.m_text));
				m_pMinecraft->m_gui.addMessage(m_pMinecraft->m_pLocalPlayer->m_name + ": " + m_textInput.m_text);
				if (m_textInput.m_text.substr(0, 3) == "/tp") {
					std::istringstream iss(m_textInput.m_text);
					std::string command;
					iss >> command;

					std::vector<int> parameters;
					int param;
					while (iss >> param) {
						parameters.push_back(param);
					}
					m_pMinecraft->m_pLocalPlayer->moveTo(parameters[0], parameters[1], parameters[2], parameters[3], parameters[4]);

				}
				
			}
			else
			{
				m_pMinecraft->m_pRakNetInstance->send(new MessagePacket(m_textInput.m_text));
			}
			

			m_textInput.m_text.clear();
		}
		
	}
}

void ChatScreen::init()
{
	m_textInput.m_width = 200;
	m_textInput.m_height = 20;
	m_textInput.m_xPos = (m_width - 250) / 16;
	m_textInput.m_yPos = m_height - 25;

	m_sendButton.m_xPos = m_textInput.m_xPos + 200;

	m_sendButton.m_yPos = m_height - 25;
	m_sendButton.m_width = 100;
	m_sendButton.m_height = 20;

	m_textInputs.push_back(&m_textInput);

	m_buttons.push_back(&m_sendButton);
	m_buttonTabList.push_back(&m_sendButton);

	m_textInput.init(m_pFont);
	
}

void ChatScreen::render(int mouseX, int mouseY, float f)
{
	Screen::renderBackground();
	Screen::render(mouseX, mouseY, f);
	glDisable(GL_DEPTH_TEST);

	fill(0, 0, m_width, m_height, 0x80000000);

	glEnable(GL_BLEND);

	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	m_pMinecraft->m_pTextures->loadAndBindTexture("gui/gui.png");

	if (m_pMinecraft->m_gui.m_guiMessages.size() > 14)
	{
		m_pMinecraft->m_gui.m_guiMessages.resize(1);
	}

	std::vector<GuiMessage> r;
	r.assign(m_pMinecraft->m_gui.m_guiMessages.rbegin(), m_pMinecraft->m_gui.m_guiMessages.rend());

	int i = 2;
	for (auto it : r) drawString(m_pFont, it.msg + "\n", m_textInput.m_xPos, (i++) * 10, 0xDDDDDD);

	glEnable(GL_DEPTH_TEST);
}
