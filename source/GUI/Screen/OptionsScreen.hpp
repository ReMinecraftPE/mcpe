#pragma once

#include "Screen.hpp"

class OptionsScreen : public Screen
{
public:
	OptionsScreen();
	void init() override;
	void render(int, int, float) override;
	void removed() override;

#ifndef ORIGINAL_CODE
	void buttonClicked(Button* pButton) override;

	void UpdateTexts();

private:
	Button m_BackButton;
	Button m_AOButton;
	Button m_srvVisButton;
	Button m_fancyGfxButton;
	Button m_invertYButton;
	Button m_anaglyphsButton;
	Button m_viewBobButton;
	Button m_viewDistButton;
	Button m_flightHaxButton;
#endif
};

