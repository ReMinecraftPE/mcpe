#pragma once

#include "Screen.hpp"

#ifndef ORIGINAL_CODE

class CreateWorldScreen : public Screen
{
public:
	CreateWorldScreen();

	void init() override;
	void buttonClicked(Button* pButton) override;
	void render(int mouseX, int mouseY, float f) override;

public:
	TextInputBox m_textName;
	TextInputBox m_textSeed;
	Button m_btnBack;
	Button m_btnCreate;
};

#endif
