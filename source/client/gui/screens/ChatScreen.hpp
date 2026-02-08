/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "../Screen.hpp"
#include "client/gui/components/Button.hpp"
#include "client/gui/components/TextBox.hpp"

class ChatScreen : public Screen
{
public:
	ChatScreen(bool slash = false);

protected:
	void _controllerDirectionHeld(GameController::StickID stickId, GameController::StickState stickState) override;
	void _buttonClicked(Button*) override;

public:
	void init() override;
	void removed() override;
	void render(float f) override;
	void keyPressed(int keyCode) override;

	void sendMessageAndExit();

private:
	TextBox m_textChat;
	Button m_btnSend;
};

