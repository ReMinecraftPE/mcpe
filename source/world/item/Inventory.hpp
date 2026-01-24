#pragma once

#include <vector>
#include "world/Container.hpp"
#include "GameMods.hpp"
#include "world/item/ItemStack.hpp"
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
	virtual ~Inventory();
	void prepareCreativeInventory();
	void prepareSurvivalInventory();

	uint16_t getContainerSize() const override;

	void clear();

	void addCreativeItem(int itemID, int auxValue = 0);
	void addTestItem(int itemID, int amount, int auxValue = 0);

	bool hasUnlimitedResource(const ItemStack& pInstance) const;

	bool add(ItemStack& instance);
    void tick();

	ItemStack& getItem(int slotNo) override;
	ItemStack& getArmor(Item::EquipmentSlot slotNo);
	ItemStack& getSelectedItem();
	int getSelectedItemId();

	void setItem(int index, const ItemStack& item);
	void setSelectedItem(ItemStack item);
	ItemStack removeItem(int index, int count) override;
	bool removeResource(int id);

	void setCarried(ItemStack item);
	ItemStack& getCarried();

	void pickItem(int itemID, int data, int maxHotBarSlot);
	void selectItem(int itemID, int maxHotBarSlot);
	void swapItems(int, int);
	void selectSlot(int slotNo);

	int getAttackDamage(Entity*);

	int getArmorValue() const;

	void hurtArmor(int amount);

	void dropAll(bool onlyClearContainer = false);

	void save(ListTag& tag) const;
	void load(const ListTag&);

	bool contains(const ItemStack&) const;

	uint16_t getSelectedSlotNo() const { return m_selectedSlot; }

	// v0.2.0 name alias
	ItemStack& getSelected() { return getSelectedItem(); }

	std::string getName() const override
	{
		//@TODO: Language
		return "Inventory";
	}

	void setChanged() override { }

	bool stillValid(Player* player) const override { return true;	}
	
private:
	GameType _getGameMode() const;
	int getSlotWithRemainingSpace(const ItemStack& item);
	int getFreeSlot();
	int addResource(const ItemStack& item);
	int getSlot(int id);

public:
	Player* m_pPlayer;
	SlotID m_selectedSlot;

private:
	ItemStack m_carried;

	std::vector<ItemStack> m_items;
	std::vector<ItemStack> m_armor;
};
