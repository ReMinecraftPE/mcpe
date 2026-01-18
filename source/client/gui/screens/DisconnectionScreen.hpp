/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "../Screen.hpp"
#include "../components/SmallButton.hpp"

class DisconnectionScreen : public Screen
{
public:
	DisconnectionScreen(const std::string& text);

	void _buttonClicked(Button* pButton) override;
	void init() override;
	void render(float f) override;

	bool isInGameScreen() override { return false; }

private:
	std::string m_text;
	Button m_btnOK;
};

