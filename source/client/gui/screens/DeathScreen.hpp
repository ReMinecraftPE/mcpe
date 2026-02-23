/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "../Screen.hpp"
#include "client/gui/components/Button.hpp"

class DeathScreen : public Screen
{
public:
	DeathScreen();

protected:
	void _buttonClicked(Button* pButton) override;

public:
	void init() override;
	void tick() override;
	void render(float f) override;

private:
	int m_tickCounter;
	Button m_btnRespawn;
	Button m_btnTitle;
};
