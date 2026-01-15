#pragma once

#include "../Screen.hpp"

class CreditsScreen : public Screen
{
public:
	CreditsScreen();
    ~CreditsScreen();

private:
	void _initCreditsText();

public:
	void init() override;
	bool isInGameScreen() override;
	void keyPressed(int code) override;
	void tick() override;
	void render(int mouseX, int mouseY, float f) override;
	bool handleBackEvent(bool b) override;
	void buttonClicked(Button* pButton) override;

public:
	Button m_btnBack;

private:
	std::vector<std::string> m_credits;
};
