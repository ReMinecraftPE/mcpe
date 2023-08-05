/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "../Screen.hpp"

class InvalidLicenseScreen : public Screen
{
public:
	InvalidLicenseScreen(int error, bool bHasQuitButton);
	void buttonClicked(Button* pButton) override;
	void init() override;
	void tick() override;
	void render(int mouseX, int mouseY, float f) override;

private:
	int m_error;
	std::string m_textLine1;
	std::string m_textLine2;
	Button m_btnOk;
	Button m_btnBuy;
	bool m_bHasQuitButton;
	int field_E4 = 0;
};

