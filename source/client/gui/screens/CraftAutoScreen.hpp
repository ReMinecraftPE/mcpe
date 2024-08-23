
#pragma once

#include "../Screen.hpp"
#include "../Components/Button.hpp"
#include "../Components/CraftButton.hpp"

class CraftAutoScreen : public Screen
{
public:
	CraftAutoScreen();
	virtual void init() override;
	virtual void render(int x, int y, float f) override;
	virtual void buttonClicked(Button*) override;
	virtual void mouseClicked(int x, int y, int type) override;
	virtual void mouseReleased(int x, int y, int type) override;
	virtual void removed() override;

private:
	Button m_btnUnknown;
	//CraftButton m_btnUnknown2;
	//Button m_btnPause;
};

