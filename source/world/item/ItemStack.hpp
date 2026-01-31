/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "Item.hpp"
#include "world/level/TilePos.hpp"
#include "world/Facing.hpp"

class Item; // in case we're included from Item.hpp
class Tile;
class Level;
class Entity;
class Mob;
class Player;
class CompoundTag;

class ItemStack
{
public:
	static const std::string TAG_DISPLAY;
	static const std::string TAG_DISPLAY_NAME;
	static const std::string TAG_REPAIR_COST;
	static const std::string TAG_ENCHANTS;

	static const ItemStack EMPTY;

private:
    void _init(int id = 0, int count = 0, int auxValue = 0);
    
public:
	ItemStack();
	ItemStack(const ItemStack& other);
	ItemStack(bool isValid);
	ItemStack(Item*);
	ItemStack(Item*, int count);
	ItemStack(Item*, int count, int auxValue);
	ItemStack(Tile*);
	ItemStack(Tile*, int count);
	ItemStack(Tile*, int count, int auxValue);
	ItemStack(int id, int count, int auxValue);
    ~ItemStack();

private:
	bool _setItem(int id);

public:
	int getId() const;
	int getIdAux() const;

    int getAuxValue() const { return m_auxValue; }
    void setAuxValue(int16_t auxValue) { m_auxValue = auxValue; } // Technically doesn't exist in b1.2_02
    int getDamageValue() const { return m_auxValue; }

	bool hasUserData() const { return m_userData != nullptr; }
	const CompoundTag* getUserData() const { return m_userData; }
	CompoundTag* getNetworkUserData() const;
	void setUserData(CompoundTag* tag);
	bool hasSameUserData(const ItemStack& other) const;

	void set(int inCount);
	bool canDestroySpecial(const Tile*);
	std::string getDescriptionId();
	std::string getHovertextName() const;
	float getDestroySpeed(const Tile*);
	int getIcon() const;
	int getMaxDamage() const;
	int getMaxStackSize() const;
	void hurt(int by);
	void hurtAndBreak(int, Entity*);
	void hurtEnemy(Mob*, Mob*);
	void interactEnemy(Mob*);
	bool isDamageableItem() const;
	bool isDamaged() const;
	bool isStackable() const;
	bool isStackedByData() const;
	void mineBlock(const TilePos& pos, Facing::Name face, Mob* mob);
	void shrink(int count = 1);
	ItemStack remove(int count);
	void setDescriptionId(const std::string&);
	void snap(Player*);
	std::string toString() const;
	ItemStack* use(Level*, Player*);
	bool useOn(Player*, Level*, const TilePos& pos, Facing::Name face);
	void onCraftedBy(Player*, Level*);
	void onCraftedBy(Player*, Level*, int amount);

	// Both need to return non-const pointers since TileRenderer calls setShape on render
	Item* getItem() const { return m_pItem; }
	Tile* getTile() const { return m_pTile; }
	bool isValid() const { return m_bValid; }
	ItemStack* copy() const;

	// v0.2.0
	int getAttackDamage(Entity *pEnt) const;
	//formerly known as isNull
	bool isEmpty() const;
	void setEmpty();

	// 0.12.1
	int getBaseRepairCost() const;
	void setRepairCost(int repairCost);
	bool hasCustomHoverName() const;
	std::string getHoverName() const;
	void setHoverName(const std::string& hoverName);

	void load(const CompoundTag& tag);
	CompoundTag& save(CompoundTag& tag) const;

	//formerly known as isNull
	static bool isEmpty(const ItemStack*);
	static bool matches(const ItemStack&, const ItemStack&);
	static ItemStack fromTag(const CompoundTag& tag);

	ItemStack* operator=(const ItemStack&);
	bool operator==(const ItemStack&) const;
	bool operator!=(const ItemStack&) const;
	operator bool() const;

public:
	int16_t m_count;
	int m_popTime;
private:
    int16_t m_auxValue;
	CompoundTag* m_userData;
	Item* m_pItem;
	Tile* m_pTile;
	bool m_bValid;
};

