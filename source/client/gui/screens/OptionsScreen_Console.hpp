#pragma once

#include "PanelScreen_Console.hpp"
#include "client/gui/components/Button.hpp"

class ControlsPanelScreen : public PanelScreen_Console
{
public:
	ControlsPanelScreen(Screen*, Minecraft*);

	void removed() override;
};

class SettingsPanelScreen : public PanelScreen_Console
{
public:
	SettingsPanelScreen(Screen*, Options&);

	void render(float) override;
	void removed() override;
};

class OptionsScreen_Console : public Screen
{
public:
	OptionsScreen_Console(Screen*);

protected:
	void _buttonClicked(Button*) override;

public:
	void init() override;
	void render(float) override;
	bool handleBackEvent(bool) override;

private:
	Screen* m_pParent;
	Button m_btnHowToPlay;
	Button m_btnControls;
	Button m_btnSettings;
	Button m_btnCredits;
	Button m_btnResetToDefaults;
};

class OptionHeader_Console : public GuiElement
{
public:
	OptionHeader_Console(const std::string& text);

public:
	void render(Minecraft* pMinecraft, const MenuPointer& pointer) override;

private:
	std::string m_text;
};

