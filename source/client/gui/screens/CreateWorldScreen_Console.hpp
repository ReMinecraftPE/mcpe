#pragma once

#include "PanelScreen_Console.hpp"
#include "client/gui/components/Button.hpp"
#include "client/gui/components/TickBox.hpp"
#include "client/gui/components/SliderButton.hpp"
#include "client/gui/components/TextBox.hpp"

class CreateWorldScreen_Console : public PanelScreen_Console
{
public:
	CreateWorldScreen_Console(Options&, Screen*);

protected:
	void _buttonClicked(Button* pButton) override;

public:
	void init() override;
	void render(float f) override;
	void renderPanel(float f) override;

public:
	TickBox m_onlineGame;
	TickBox m_inviteOnly;
	TextBox m_textName;
	TextBox m_textSeed;
	SliderButton m_difficultySlider;
	Button m_btnCreate;
};