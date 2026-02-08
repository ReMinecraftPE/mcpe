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

class DirectConnectScreen : public Screen
{
public:
	DirectConnectScreen();
	
protected:
	void _buttonClicked(Button* pButton) override;

public:
	void init() override;
	void render(float f) override;
	bool handleBackEvent(bool b) override;
	void onTextBoxUpdated(int id) override;

private:
	TextBox m_textAddress;
	Button m_btnCancel;
	Button m_btnJoin;
};
