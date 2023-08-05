/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "../Screen.hpp"

class StartMenuScreen : public Screen
{
public:
	StartMenuScreen();
	void _updateLicense();

	void init() override;
	void buttonClicked(Button*) override;
	bool isInGameScreen() override;
	void render(int, int, float) override;
	void tick() override;

private:
	Button m_startButton;
	Button m_joinButton;
	Button m_optionsButton;
	Button m_testButton;
	Button m_buyButton;
	std::string field_154;
	int field_16C;
	std::string field_170;
	int field_188;

	//TextInputBox m_testBox;
};

