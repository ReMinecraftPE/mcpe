#pragma once

#include <vector>
#include "world/Container.hpp"
#include "GameMods.hpp"
#include "world/item/ItemInstance.hpp"
#include "world/entity/Player.hpp"
#include "world/gamemode/GameType.hpp"
#include "nbt/ListTag.hpp"

class Entity;
class Player; // in case we're included from Player.hpp


#define C_POP_TIME_DURATION (5)
#define C_MAX_HOTBAR_ITEMS (9)
#define C_NUM_INVENTORY_SLOTS (36)
#define C_NUM_ARMOR_SLOTS (4)

class Inventory : public Container
{
public:
	Inventory(Player*);
	~Inventory();
	void prepareCreativeInventory();
	void prepareSurvivalInventory();

	int getContainerSize() const override;

	void clear();

	void addCreativeItem(int itemID, int auxValue = 0);
	void addTestItem(int itemID, int amount, int auxValue = 0);

	bool hasUnlimitedResource(const ItemInstance& pInstance) const;

	bool add(ItemInstance& instance);
    void tick();

	ItemInstance& getItem(int slotNo) const override;
	ItemInstance& getSelectedItem() const;
	int getSelectedItemId() const;

	void setItem(int index, const ItemInstance& item);
	void setSelectedItem(ItemInstance item);
	ItemInstance removeItem(int index, int count) override;
	bool removeResource(int id);

	void setCarried(ItemInstance item);
	ItemInstance& getCarried();

	void selectItem(int itemID, int data, int maxHotBarSlot); // selects an item by slot number and puts it in the quick slots if needed
	void selectSlot(int slotNo);

	int getAttackDamage(Entity*);

	void dropAll(bool onlyClearContainer = false);

	void save(ListTag& tag) const;
	void load(const ListTag&);

	bool contains(const ItemInstance&) const;

	int getSelectedSlotNo() const { return m_selected; }

	// v0.2.0 name alias
	ItemInstance& getSelected() { return getSelectedItem(); }

	std::string getName() const override
	{
		//@TODO: Language
		return "Inventory";
	}

	void setChanged() override
	{

	}

	bool stillValid(Player* player) override
	{
		return true;
	}
	
private:
	GameType _getGameMode() const;
	int getSlotWithRemainingSpace(const ItemInstance& item);
	int getFreeSlot();
	int addResource(const ItemInstance& item);
	int getSlot(int id);

public:
	int m_selected;
private:
	Player* m_pPlayer;
	ItemInstance m_carried;

	mutable std::vector<ItemInstance> m_armor;
	mutable std::vector<ItemInstance> m_items;
};
