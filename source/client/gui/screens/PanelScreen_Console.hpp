#pragma once

#include "../Screen.hpp"
#include "client/gui/components/Button.hpp"

#include "../VerticalLayout.hpp"

class PanelScreen_Console : public Screen
{
public:
	PanelScreen_Console(Screen*);

public:
	void init() override;
	void render(float f) override;
	virtual void renderPanel(float f);
	bool handleBackEvent(bool b) override;

public:
	Screen* m_pParent;
	IntRectangle m_panel;
	VerticalLayout m_layout;
};
