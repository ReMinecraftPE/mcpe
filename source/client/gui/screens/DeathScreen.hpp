/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "../Screen.hpp"

class DeathScreen : public Screen
{
public:
	DeathScreen();

	virtual void init() override;
	virtual void buttonClicked(Button* pButton) override;
	virtual void tick() override;
	virtual void keyPressed(int key) override;
	virtual void render(int x, int y, float f) override;

private:
	int m_tickCounter;
	Button m_btnRespawn;
	Button m_btnTitle;
};
