/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "../Screen.hpp"

#define BUTTON_HEIGHT 20
#define BUTTON_GAP_HEIGHT 5
#define BUTTON_GAP (BUTTON_HEIGHT + BUTTON_GAP_HEIGHT)

#define TOP_CUTOFF 35
#define BOTTOM_CUTOFF 40

struct RebindingOption_t
{
	Button* button;
	int ypos;
};

const std::string keymapToString[] = 
{
	"Move Forward",
	"Move Left",
	"Move Back",
	"Move Right",
	"Jump",
	"Open Inventory",
	"Drop Held Item",
	"Open Chat",
	"Toggle Fog",
	"Sneak",
	"Destroy",
	"Place",
	"Menu Next",
	"Menu Previous",
	"Menu OK",
	"Pause",
	"Select Slot 1",
	"Select Slot 2",
	"Select Slot 3",
	"Select Slot 4",
	"Select Slot 5",
	"Select Slot 6",
	"Select Slot 7",
	"Select Slot 8",
	"Select Slot 9",
	"Select Left Slot",
	"Select Right Slot",
	"Toggle GUI",
	"Take Screenshot",
	"Toggle Debug Info",
	"Toggle AO",
	"Toggle Thirdperson",
	"Fly Up",
	"Fly Down",
	"Open Command Chat"
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

	RebindingOption_t m_rebindingOptions[KM_COUNT];

	Button m_btnBack;
};

