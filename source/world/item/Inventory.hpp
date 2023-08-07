#pragma once

#include <vector>
#include "world/item/ItemInstance.hpp"
#include "world/entity/Player.hpp"

class Player; // in case we're included from Player.hpp

#define C_MAX_HOTBAR_ITEMS (9)
#define C_NUM_SURVIVAL_SLOTS (36)
#define C_MAX_AMOUNT (64)

class Inventory
{
public:
	Inventory(Player*);
	void prepareCreativeInventory();
	void prepareSurvivalInventory();

	int getNumSlots();
	int getNumItems();

	void addCreativeItem(int itemID, int auxValue = 0);
	void addTestItem(int itemID, int amount, int auxValue = 0);

	void clear();
	void addItem(ItemInstance* pInst);

	ItemInstance* getItem(int slotNo);
	ItemInstance* getQuickSlotItem(int slotNo);
	ItemInstance* getSelectedItem();
	int getQuickSlotItemId(int slotNo);
	int getSelectedItemId();
	void selectItem(int slotNo); // selects an item by slot number and puts it in the quick slots if needed
	void selectSlot(int slotNo);

	void setQuickSlotIndexByItemId(int slotNo, int itemID);
	void selectItemById(int itemID);

	int getSelectedSlotNo() const
	{
		return m_SelectedHotbarSlot;
	}

public:
	int m_SelectedHotbarSlot;
private:
	Player* m_pPlayer;
	bool m_bIsSurvival;

	int m_hotbar[C_MAX_HOTBAR_ITEMS];
	std::vector<ItemInstance> m_items;
};

