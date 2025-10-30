#pragma once

#include <vector>
#include "GameMods.hpp"
#include "world/item/ItemInstance.hpp"
#include "world/entity/Player.hpp"
#include "world/gamemode/GameType.hpp"
#include "nbt/ListTag.hpp"

class Entity;
class Player; // in case we're included from Player.hpp

#define C_POP_TIME_DURATION (5)
#define C_MAX_HOTBAR_ITEMS (9)
#ifdef MOD_POCKET_SURVIVAL
#define C_SURVIVAL_INVENTORY_SIZE (42)
#else
#define C_SURVIVAL_INVENTORY_SIZE (36)
#endif
#define C_MAX_INVENTORY_STACK_SIZE (64)

class Inventory
{
public:
	Inventory(Player*);
	~Inventory();
	void prepareCreativeInventory();
	void prepareSurvivalInventory();

	int getNumSlots();
	int getNumItems();

	void addCreativeItem(int itemID, int auxValue = 0);
	void addTestItem(int itemID, int amount, int auxValue = 0);

	bool hasUnlimitedResource(const ItemInstance* pInstance) const;

	void release(int slotNo);
	void empty();
	void clear();
	bool addItem(ItemInstance& instance);
    void tick();

	ItemInstance* getItem(int slotNo) const;
	ItemInstance* getQuickSlotItem(int slotNo) const;
	ItemInstance* getSelectedItem() const;
	int getQuickSlotItemId(int slotNo) const;
	int getSelectedItemId() const;

	void setItem(int index, ItemInstance* item);
	void setSelectedItem(ItemInstance* item);

	void selectItem(int slotNo, int maxHotBarSlot); // selects an item by slot number and puts it in the quick slots if needed
	void selectSlot(int slotNo);
	void setQuickSlotIndexByItemId(int slotNo, int itemID);
	void selectItemById(int itemID, int maxHotBarSlot);
	void selectItemByIdAux(int itemID, int auxValue, int maxHotBarSlot);

	int getAttackDamage(Entity*);

	void dropAll(bool onlyClearContainer = false);

	void save(ListTag& tag) const;
	void load(const ListTag&);

	int getSelectedSlotNo() const { return m_selectedHotbarSlot; }

	// v0.2.0 name alias
	ItemInstance* getSelected() { return getSelectedItem(); }
	
private:
	GameType _getGameMode() const;

public:
	int m_selectedHotbarSlot;
private:
	Player* m_pPlayer;

	int m_hotbar[C_MAX_HOTBAR_ITEMS];
	std::vector<ItemInstance*> m_items;
};
