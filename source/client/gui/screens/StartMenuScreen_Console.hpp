#pragma once

#include "PanelScreen.hpp"
#include "client/gui/components/Button.hpp"

class StartMenuScreen_Console : public Screen
{
public:
	StartMenuScreen_Console();
	void init() override;
	void render(float) override;
	void _buttonClicked(Button*) override;

private:
	Button m_btnPlayGame;
	Button m_btnLeaderboards;
	Button m_btnAchievements;
	Button m_btnHelpAndOptions;
	Button m_btnDownload;
	Button m_btnExitGame;
	std::string m_splash;
};
