#pragma once

#include <vector>
#include "world/item/ItemInstance.hpp"
#include "world/entity/Player.hpp"

class Entity;
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

	unsigned int getNumSlots();
	unsigned int getNumItems();

	void addCreativeItem(unsigned int itemID, int auxValue = 0);
	void addTestItem(unsigned int itemID, int amount, int auxValue = 0);

	void clear();
	void addItem(ItemInstance* pInst);

	ItemInstance* getItem(unsigned int slotNo);
	ItemInstance* getQuickSlotItem(unsigned int slotNo);
	ItemInstance* getSelectedItem();
	unsigned int getQuickSlotItemId(unsigned int slotNo);
	unsigned int getSelectedItemId();
	void selectItem(unsigned int slotNo, unsigned int maxHotBarSlot); // selects an item by slot number and puts it in the quick slots if needed
	void selectSlot(unsigned int slotNo);

	void setQuickSlotIndexByItemId(unsigned int slotNo, unsigned int itemID);
	void selectItemById(unsigned int itemID, unsigned int maxHotBarSlot);

	int getAttackDamage(Entity*);

	unsigned int getSelectedSlotNo() const
	{
		return m_selectedHotbarSlot;
	}

	// v0.2.0 name alias
	ItemInstance* getSelected() {
		return getSelectedItem();
	}

public:
	unsigned int m_selectedHotbarSlot;
private:
	Player* m_pPlayer;
	bool m_bIsSurvival;

	unsigned int m_hotbar[C_MAX_HOTBAR_ITEMS];
	std::vector<ItemInstance> m_items;
};

