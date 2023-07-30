#pragma once

#include "Screen.hpp"

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

