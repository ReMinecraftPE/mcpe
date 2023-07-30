#pragma once

#include "Screen.hpp"

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

