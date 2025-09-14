/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "common/Utils.hpp"
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

class ItemInstance
{
private:
    void _init(int itemID, int amount, int auxValue);
    
public:
	ItemInstance();
	ItemInstance(Item*);
	ItemInstance(Item*, int amount);
	ItemInstance(Item*, int amount, int auxValue);
	ItemInstance(Tile*);
	ItemInstance(Tile*, int amount);
	ItemInstance(Tile*, int amount, int auxValue);
	ItemInstance(int itemID, int amount, int auxValue);
    ~ItemInstance();

	int getId() const;
	int getIdAux() const;

    int getAuxValue() const { return m_auxValue; }
    void setAuxValue(int16_t auxValue) { m_auxValue = auxValue; } // Technically doesn't exist in b1.2_02
    int getDamageValue() const { return m_auxValue; }

	bool hasUserData() const { return m_userData != nullptr; }
	const CompoundTag* getUserData() const { return m_userData; }
	void setUserData(CompoundTag* tag);

	void set(int inCount);
	bool canDestroySpecial(Tile*);
	std::string getDescriptionId();
	float getDestroySpeed(Tile*);
	int getIcon() const;
	int getMaxDamage() const;
	int getMaxStackSize() const;
	void hurt(int by);
	void hurtEnemy(Mob*);
	void interactEnemy(Mob*);
	bool isDamageableItem();
	bool isDamaged();
	bool isStackable();
	bool isStackedByData();
	void mineBlock(const TilePos& pos, Facing::Name face);
	ItemInstance remove(int amt);
	void setDescriptionId(const std::string&);
	void snap(Player*);
	std::string toString();
	ItemInstance* use(Level*, Player*);
	bool useOn(Player*, Level*, const TilePos& pos, Facing::Name face);

	Item* getItem() const;
	ItemInstance* copy() const;

	// v0.2.0
	int getAttackDamage(Entity *pEnt);
	bool isNull() const;
	void setNull();

	void load(const CompoundTag& tag);
	CompoundTag& save(CompoundTag& tag) const;

	// @NOTE: Won't this be ambiguous with the non-static method?
	static bool isNull(const ItemInstance*);
	static bool matches(const ItemInstance*, const ItemInstance*);
	static ItemInstance* fromTag(const CompoundTag& tag);

	bool operator==(const ItemInstance&) const;
	bool operator!=(const ItemInstance&) const;
	//operator bool() const;

public:
	int16_t m_count;
	int m_popTime;
	int16_t m_itemID;
private:
    int16_t m_auxValue;
	CompoundTag* m_userData;
	//Item* m_item; // @TODO: replace m_itemID with Item pointer
	//bool m_valid;
};

