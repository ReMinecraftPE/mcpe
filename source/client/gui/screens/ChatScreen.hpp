/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "Screen.hpp"
#include "../../../windows_vs/Commands.hpp"

class ChatScreen : public Screen
{
public:
	Button m_sendButton;
	ChatScreen();
	void init();
	void render(int mouseX, int mouseY, float f) override;
	// void keyPressed(int);
	TextInputBox m_textInput;
	void buttonClicked(Button* pButton);
private:
	std::vector<std::string> m_chatLog;

	
};
