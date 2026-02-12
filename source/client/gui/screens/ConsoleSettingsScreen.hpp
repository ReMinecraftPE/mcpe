#pragma once

#include "PanelScreen.hpp"
#include "client/gui/components/Button.hpp"

class ControlsPanelScreen : public PanelScreen
{
public:
	ControlsPanelScreen(Screen*, Minecraft*);

	void removed() override;
};

class SettingsPanelScreen : public PanelScreen
{
public:
	SettingsPanelScreen(Screen*, Options&);

	void render(float) override;
	void removed() override;
};

class ConsoleSettingsScreen : public Screen
{
public:
	ConsoleSettingsScreen(Screen*);
	void init() override;
	void render(float) override;
	bool handleBackEvent(bool) override;
	virtual void _buttonClicked(Button*) override;

private:
	Screen* m_pParent;
	Button m_btnHowToPlay;
	Button m_btnControls;
	Button m_btnSettings;
	Button m_btnCredits;
	Button m_btnResetToDefaults;
};

