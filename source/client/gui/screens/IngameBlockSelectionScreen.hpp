/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "../Screen.hpp"

class Inventory;

class IngameBlockSelectionScreen : public Screen
{
public:
	IngameBlockSelectionScreen();

	Inventory* getInventory();
	int getBottomY();
	int getSelectedSlot(int x, int y);
	int getSlotPosX(int x);
	int getSlotPosY(int y);
	int getSlotsHeight();
	bool isAllowed(int slot);
	bool isInsideSelectionArea(int x, int y);
	void renderSlots();
	void renderDemoOverlay();
	void renderSlot(int index, int x, int y, float f);
	void selectSlotAndClose();

	virtual void init() override;
	virtual void render(int x, int y, float f) override;
    virtual void buttonClicked(Button*) override;
	virtual void mouseClicked(int x, int y, int type) override;
	virtual void mouseReleased(int x, int y, int type) override;
	virtual void removed() override;

private:
	int m_selectedSlot;
	bool m_bReleased;
	bool m_bClickedOnSlot;
    Button m_btnPause;
	Button m_btnChat;
	Button m_btnCraft;
	Button m_btnArmor;
};

