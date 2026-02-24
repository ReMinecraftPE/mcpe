/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "../Screen.hpp"
#include "client/gui/components/Button.hpp"

class PauseScreen : public Screen
{
public:
	PauseScreen();
	void init() override;
	void tick() override;
	void render(float f) override;
	void _buttonClicked(Button*) override;
	bool isInvalid(Minecraft*) override;
	
	void updateServerVisibilityText();

private:
	//int m_oPos;
	int field_40;
	Button m_btnBack;
	Button m_btnQuit;
	Button m_btnQuitAndCopy;
	Button m_btnVisible;

#ifdef ENH_ADD_OPTIONS_PAUSE
	Button m_btnOptions;
#endif
};

