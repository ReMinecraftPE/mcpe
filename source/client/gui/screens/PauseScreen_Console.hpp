#pragma once

#include "PanelScreen.hpp"
#include "client/gui/components/Button.hpp"

class PauseScreen_Console : public Screen
{
public:
	PauseScreen_Console();
	void init() override;
	void render(float) override;
	void _buttonClicked(Button*) override;

private:
	Button m_btnResume;
	Button m_btnHelpAndOptions;
	Button m_btnLeaderboards;
	Button m_btnAchievements;
	Button m_btnSaveGame;
	Button m_btnExitGame;
};
