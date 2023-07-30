/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include <sstream>
#include "ItemInstance.hpp"
#include "Tile.hpp"

void ItemInstance::init(int itemID, int amount, int auxValue)
{
	m_itemID = itemID;
	m_amount = amount;

	//@BUG? Not using the auxValue.  This is problematic in the case of wool and dyes.
}

ItemInstance::ItemInstance(Item* pItem)
{
	init(pItem->m_itemID, 1, 0);
}

ItemInstance::ItemInstance(Item* pItem, int amount)
{
	init(pItem->m_itemID, amount, 0);
}

ItemInstance::ItemInstance(Item* pItem, int amount, int auxValue)
{
	init(pItem->m_itemID, amount, auxValue);
}

ItemInstance::ItemInstance(Tile* pTile)
{
	init(pTile->m_ID, 1, 0);
}

ItemInstance::ItemInstance(Tile* pTile, int amount)
{
	init(pTile->m_ID, amount, 0);
}

ItemInstance::ItemInstance(Tile* pTile, int amount, int auxValue)
{
	init(pTile->m_ID, amount, auxValue);
}

ItemInstance::ItemInstance(int itemID, int amount, int auxValue)
{
	init(itemID, amount, auxValue);
}

int ItemInstance::getAuxValue()
{
	return m_auxValue;
}

int ItemInstance::getDamageValue()
{
	return m_auxValue;
}

Item* ItemInstance::getItem()
{
	return Item::items[m_itemID];
}

ItemInstance* ItemInstance::copy()
{
	return new ItemInstance(m_itemID, m_amount, m_auxValue);
}

bool ItemInstance::canDestroySpecial(Tile* tile)
{
	return getItem()->canDestroySpecial(tile);
}

std::string ItemInstance::getDescriptionId()
{
	return getItem()->getDescriptionId(this);
}

float ItemInstance::getDestroySpeed(Tile* tile)
{
	return getItem()->getDestroySpeed(this, tile);
}

int ItemInstance::getIcon()
{
	return getItem()->getIcon(this);
}

int ItemInstance::getMaxDamage()
{
	return getItem()->getMaxDamage();
}

int ItemInstance::getMaxStackSize()
{
	return getItem()->getMaxStackSize();
}

void ItemInstance::hurt(int by)
{
	if (!isDamageableItem())
		return;

	m_auxValue += by;
	if (m_auxValue > getMaxDamage())
	{
		m_amount--;
		if (m_amount < 0)
			m_amount = 0;
		m_auxValue = 0;
	}
}

void ItemInstance::hurtEnemy(Mob* mob)
{
	getItem()->hurtEnemy(this, mob);
}

void ItemInstance::interactEnemy(Mob* mob)
{
	getItem()->interactEnemy(this, mob);
}

bool ItemInstance::isDamageableItem()
{
	return getItem()->getMaxDamage() > 0;
}

bool ItemInstance::isDamaged()
{
	return m_auxValue > 0;
}

bool ItemInstance::isStackable()
{
	if (getMaxStackSize() <= 1)
		return false;

	if (isDamageableItem())
		return !isDamaged();

	return true;
}

bool ItemInstance::isStackedByData()
{
	return getItem()->isStackedByData();
}

bool ItemInstance::matches(ItemInstance* other) const
{
	return m_auxValue == other->m_auxValue && m_amount == other->m_amount && m_itemID == other->m_itemID;
}

bool ItemInstance::matches(ItemInstance* a1, ItemInstance* a2)
{
	if (a1 == a2 && a1 == nullptr)
		return true;

	if (a1 == nullptr || a2 == nullptr)
		return false;

	return a1->matches(a2);
}

void ItemInstance::mineBlock(int x, int y, int z, int d)
{
	return getItem()->mineBlock(this, x, y, z, d);
}

ItemInstance ItemInstance::remove(int amt)
{
	m_amount -= amt;
	return ItemInstance(m_itemID, amt, m_auxValue);
}

void ItemInstance::setDescriptionId(const std::string& str)
{
}

void ItemInstance::snap(Player*)
{

}

std::string ItemInstance::toString()
{
	std::stringstream ss;
	ss << m_amount << "x" << getItem()->getDescriptionId() << "@" << m_auxValue;

	return ss.str();
}

ItemInstance* ItemInstance::use(Level* level, Player* player)
{
	return getItem()->use(this, level, player);
}

bool ItemInstance::useOn(Player* player, Level* level, int x, int y, int z, int dir)
{
	return getItem()->useOn(this, player, level, x, y, z, dir);
}
