#pragma once

#include "../Screen.hpp"
#include "client/gui/components/Button.hpp"

#include "../VerticalLayout.hpp"

class PanelScreen : public Screen
{
public:
	PanelScreen(Screen*);
	~PanelScreen();

public:
	void init() override;
	void render(float f) override;
	virtual bool handleBackEvent(bool b) override;

public:
	Screen* m_pParent;
	IntRectangle m_panel;
	VerticalLayout m_layout;
};
