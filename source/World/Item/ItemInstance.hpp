/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "Utils.hpp"

#include "Item.hpp"
class Item; // in case we're included from Item.hpp
class Tile;
class Level;
class Mob;
class Player;

class ItemInstance
{
public:
	ItemInstance(Item*);
	ItemInstance(Item*, int amount);
	ItemInstance(Item*, int amount, int auxValue);
	ItemInstance(Tile*);
	ItemInstance(Tile*, int amount);
	ItemInstance(Tile*, int amount, int auxValue);
	ItemInstance(int itemID, int amount, int auxValue);

	void init(int itemID, int amount, int auxValue);

	int getAuxValue();
	int getDamageValue();

	bool canDestroySpecial(Tile*);
	std::string getDescriptionId();
	float getDestroySpeed(Tile*);
	int getIcon();
	int getMaxDamage();
	int getMaxStackSize();
	void hurt(int by);
	void hurtEnemy(Mob*);
	void interactEnemy(Mob*);
	bool isDamageableItem();
	bool isDamaged();
	bool isStackable();
	bool isStackedByData();
	bool matches(ItemInstance*) const;
	void mineBlock(int, int, int, int);
	ItemInstance remove(int amt);
	void setDescriptionId(const std::string&);
	void snap(Player*);
	std::string toString();
	ItemInstance* use(Level*, Player*);
	bool useOn(Player*, Level*, int x, int y, int z, int dir);

	Item* getItem();
	ItemInstance* copy();

	static bool matches(ItemInstance*, ItemInstance*);

public:
	int m_auxValue;
	int m_amount;
	int m_field_8;
	int m_itemID;
};

