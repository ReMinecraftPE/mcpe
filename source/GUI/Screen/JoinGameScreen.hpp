#pragma once

#include "Screen.hpp"
#include "AvailableGamesList.hpp"

class JoinGameScreen : public Screen
{
public:
	JoinGameScreen();
	~JoinGameScreen();
	void buttonClicked(Button* pButton) override;
	bool handleBackEvent(bool b) override;
	void init() override;
	bool isInGameScreen() override;
	void render(int mouseX, int mouseY, float f) override;
	void tick() override;

	virtual bool isIndexValid(int idx);

public:
	Button m_btnJoin;
	Button m_btnBack;
	AvailableGamesList* m_pAvailableGamesList = nullptr;
};

