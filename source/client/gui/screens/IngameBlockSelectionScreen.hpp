/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "../Screen.hpp"
#include "client/gui/components/Button.hpp"
#include "world/item/ItemStack.hpp"

class Inventory;

class IngameBlockSelectionScreen : public Screen
{
public:
	IngameBlockSelectionScreen();

	void addCreativeItem(int itemID, int auxValue = 0);

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

	void init() override;
	void render(float f) override;
    void _buttonClicked(Button*) override;
	void pointerPressed(const MenuPointer& pointer, MouseButtonType btn) override;
	void pointerReleased(const MenuPointer& pointer, MouseButtonType btn) override;
	void removed() override;
    void keyPressed(int key) override;

private:
	SlotID m_selectedSlot;
	bool m_bReleased;
	bool m_bClickedOnSlot;
    Button m_btnPause;
	Button m_btnChat;
	Button m_btnCraft;
	Button m_btnArmor;
	std::vector<ItemStack> m_items;
};

