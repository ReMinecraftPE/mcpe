/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "../Screen.hpp"
#include "../components/AvailableGamesList.hpp"

class JoinGameScreen : public Screen
{
public:
	JoinGameScreen();
	~JoinGameScreen();
	void buttonClicked(Button* pButton) override;
	bool handleBackEvent(bool b) override;
	void init() override;
	bool isInGameScreen() override;
	void render(int mouseX, int mouseY, float f) override;
	void tick() override;

	virtual bool isIndexValid(int idx);

public:
	Button m_btnJoin;
	Button m_btnBack;
	AvailableGamesList* m_pAvailableGamesList = nullptr;
};

