#pragma once

#include "Player.hpp"
#include "ItemInstance.hpp"

class Player; // in case we're included from Player.hpp

#define C_MAX_HOTBAR_ITEMS (9)

#define C_NUM_SURVIVAL_SLOTS (36)

class Inventory
{
public:
	Inventory(Player*);
	void prepareCreativeInventory();
	void prepareSurvivalInventory();

	int getNumSlots();
	int getNumItems();

	void addCreativeItem(int itemID, int auxValue = 0);

	ItemInstance* getItem(int slotNo);
	int getQuickSlotItemId(int slotNo);
	int getSelectedItemId();
	void selectItem(int slotNo); // selects an item by slot number and puts it in the quick slots if needed
	void selectSlot(int slotNo);

	void setQuickSlotIndexByItemId(int slotNo, int itemID);
	void selectItemById(int itemID);

public:
	int m_SelectedHotbarSlot = 0;
	Player* m_pPlayer = nullptr;
	bool m_bIsSurvival = false;

	int m_hotbar[C_MAX_HOTBAR_ITEMS];
	std::vector<ItemInstance> m_items;
};

