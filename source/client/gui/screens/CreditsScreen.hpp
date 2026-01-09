/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "../Screen.hpp"
#include "../components/WorldSelectionList.hpp"

class CreditsScreen : public Screen
{
public:
	CreditsScreen();
    ~CreditsScreen();

	void init() override;
	bool isInGameScreen() override;
	void keyPressed(int code) override;
	void tick() override;
	void render(int mouseX, int mouseY, float f) override;
	bool handleBackEvent(bool b) override;
	void buttonClicked(Button* pButton) override;

public:
	Button m_btnBack;
	WorldSelectionList* m_pDarkBackground;
};
