/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include <sstream>
#include "ItemInstance.hpp"
#include "world/tile/Tile.hpp"

void ItemInstance::init(int itemID, int amount, int auxValue)
{
	m_itemID = itemID;
	m_amount = amount;
	m_auxValue = auxValue;

	//@BUG? Not using the auxValue.  This is problematic in the case of wool and dyes.
}

ItemInstance::ItemInstance()
{
	init(0, 0, 0);
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

Item* ItemInstance::getItem() const
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

int ItemInstance::getIcon() const
{
	return getItem()->getIcon(this);
}

int ItemInstance::getMaxDamage() const
{
	return getItem()->getMaxDamage();
}

int ItemInstance::getMaxStackSize() const
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

int ItemInstance::getAttackDamage(Entity *pEnt)
{
	return getItem()->getAttackDamage(pEnt);
}

void ItemInstance::mineBlock(const TilePos& pos, Facing::Name face)
{
	return getItem()->mineBlock(this, pos, face);
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

bool ItemInstance::useOn(Player* player, Level* level, const TilePos& pos, Facing::Name face)
{
	return getItem()->useOn(this, player, level, pos, face);
}

bool ItemInstance::isNull() const
{
	// 0.9.2
	//if (!m_field_10)
	//	return true;

	if (this != nullptr)
		return false;
	if (m_amount != 0)
		return false;
	if (m_field_8 != 0)
		return false;

	return true; // isNull
}