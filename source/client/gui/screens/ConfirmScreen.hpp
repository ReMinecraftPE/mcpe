/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "../Screen.hpp"
#include "../components/SmallButton.hpp"

class ConfirmScreen : public Screen
{
public:
	ConfirmScreen(Screen* pScreen, const std::string& line1, const std::string& line2, int x);
	ConfirmScreen(Screen* pScreen, const std::string& line1, const std::string& line2, const std::string& ok, const std::string& cancel, int x);

	void buttonClicked(Button* pButton) override;
	bool handleBackEvent(bool b) override;
	void init() override;
	void render(int mouseX, int mouseY, float f) override;

	virtual void postResult(bool b);

private:
	Screen* m_pScreen = nullptr;
	int field_40 = 0;
	std::string m_textLine1;
	std::string m_textLine2;
	SmallButton m_btnOK;
	SmallButton m_btnCancel;
};

