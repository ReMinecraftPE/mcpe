/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "../Screen.hpp"

#define BUTTON_HEIGHT 20
#define BUTTON_WIDTH 80
#define BUTTON_GAP_HEIGHT 5
#define BUTTON_GAP (BUTTON_HEIGHT + BUTTON_GAP_HEIGHT)

#define TOP_CUTOFF 35
#define BOTTOM_CUTOFF 40

struct RebindingOption
{
	Button* button;
	int ypos;
};

class ControlsScreen : public Screen
{
public:
	ControlsScreen();
	virtual ~ControlsScreen();
	virtual void init() override;
	virtual void render(int a, int b, float c) override;
	virtual void buttonClicked(Button*) override;
	virtual void keyPressed(int keycode) override;
	virtual void mouseEvent() override;

	void		 UpdateText();
	void		 GetRenderableButtons();
	std::string GetCharacterText(int keycode);

private:
	bool m_bChangingKeybinds = false;

	/// <summary>
	/// This is the index of the keybind that will be set to the new keycode
	/// </summary>
	int m_iCurKeybindIndex;

	int m_iFirstRenderable;
	int m_iLastRenderable;
	int m_iScrollHeight;
	int m_iHighestScrollHeight;

	RebindingOption m_rebindingOptions[KM_COUNT];

	Button m_btnBack;
};

