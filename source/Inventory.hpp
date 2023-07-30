#pragma once

#include "Player.hpp"

class Player; // in case we're included from Player.hpp

#define C_MAX_HOTBAR_ITEMS (9)

#ifdef ENH_EXTRA_ITEMS_IN_INV
#define C_MAX_INVENTORY_ITEMS (36+9)
#else
#define C_MAX_INVENTORY_ITEMS (36)
#endif

class Inventory
{
public:
	Inventory(Player*);

	int getSelectionSize();
	int getSelectionSlotItemId(int slotNo);
	int getSelectedItemId();
	void selectSlot(int slotNo);
	void setSelectionSlotItemId(int slotNo, int item);

public:
	int m_SelectedHotbarSlot;
	Player* m_pPlayer;

	int m_hotbar[C_MAX_HOTBAR_ITEMS];
	int m_items [C_MAX_INVENTORY_ITEMS];
};

