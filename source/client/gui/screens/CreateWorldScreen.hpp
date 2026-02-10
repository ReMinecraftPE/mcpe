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

#ifndef ORIGINAL_CODE

class CreateWorldScreen : public Screen
{
public:
	CreateWorldScreen();

protected:
	void _buttonClicked(Button* pButton) override;

public:
	void init() override;
	void render(float f) override;
	bool handleBackEvent(bool b) override;

public:
	TextBox m_textName;
	TextBox m_textSeed;
	Button m_btnBack;
	Button m_btnCreate;

	int m_selectedGameType;
};

#endif
