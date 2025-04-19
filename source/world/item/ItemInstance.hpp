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

    int getAuxValue() const { return m_auxValue; }
    void setAuxValue(int auxValue) { m_auxValue = auxValue; } // Technically doesn't exist in b1.2_02
    int getDamageValue() const { return m_auxValue; }

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
	bool matches(ItemInstance*) const;
	void mineBlock(const TilePos& pos, Facing::Name face);
	ItemInstance remove(int amt);
	void setDescriptionId(const std::string&);
	void snap(Player*);
	std::string toString();
	ItemInstance* use(Level*, Player*);
	bool useOn(Player*, Level*, const TilePos& pos, Facing::Name face);

	Item* getItem() const;
	ItemInstance* copy();

	static bool matches(ItemInstance*, ItemInstance*);

	// v0.2.0
	int getAttackDamage(Entity *pEnt);
	bool isNull() const;

public:
	int m_count;
	int m_popTime;
	int m_itemID;
private:
    int m_auxValue;
};

