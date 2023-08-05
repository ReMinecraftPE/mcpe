/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "../Screen.hpp"

class PauseScreen : public Screen
{
public:
	PauseScreen();
	virtual void init() override;
	virtual void tick() override;
	virtual void render(int a, int b, float c) override;
	virtual void buttonClicked(Button*) override;
	
	void updateServerVisibilityText();

private:
	int field_3C = 0;
	int field_40 = 0;
	Button m_btnBack;
	Button m_btnQuit;
	Button m_btnQuitAndCopy;
	Button m_btnVisible;

#ifdef ENH_ADD_OPTIONS_PAUSE
	Button m_btnOptions;
#endif
};

