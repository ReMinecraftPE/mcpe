#pragma once

#include "../Screen.hpp"
#include "client/gui/components/Button.hpp"

class CreditsScreen : public Screen
{
public:
	CreditsScreen(Screen* parent);
	~CreditsScreen();

private:
	void _initCreditsText();

public:
	void init() override;
	bool isInGameScreen() override;
	void keyPressed(int code) override;
	void tick() override;
	void render(float f) override;
	bool handleBackEvent(bool b) override;
	void _buttonClicked(Button* pButton) override;

public:
	Button m_btnBack;

private:
	Screen* m_pParent;
	std::vector<std::string> m_credits;
};
