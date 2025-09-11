/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include <sstream>
#include "ItemInstance.hpp"
#include "world/tile/Tile.hpp"
#include "nbt/CompoundTag.hpp"

void ItemInstance::_init(int itemID, int count, int auxValue)
{
	m_itemID = itemID;
	m_count = count;
	m_auxValue = auxValue;
    m_userData = nullptr;
	m_popTime = 0;
}

ItemInstance::ItemInstance()
{
	_init(0, 0, 0);
}

ItemInstance::ItemInstance(Item* pItem)
{
	_init(pItem->m_itemID, 1, 0);
}

ItemInstance::ItemInstance(Item* pItem, int amount)
{
	_init(pItem->m_itemID, amount, 0);
}

ItemInstance::ItemInstance(Item* pItem, int amount, int auxValue)
{
	_init(pItem->m_itemID, amount, auxValue);
}

ItemInstance::ItemInstance(Tile* pTile)
{
	_init(pTile->m_ID, 1, 0);
}

ItemInstance::ItemInstance(Tile* pTile, int amount)
{
	_init(pTile->m_ID, amount, 0);
}

ItemInstance::ItemInstance(Tile* pTile, int amount, int auxValue)
{
	_init(pTile->m_ID, amount, auxValue);
}

ItemInstance::ItemInstance(int itemID, int amount, int auxValue)
{
	_init(itemID, amount, auxValue);
}

ItemInstance::~ItemInstance()
{
    SAFE_DELETE(m_userData);
}

int ItemInstance::getId() const
{
	return m_itemID;

	/*
	if (!m_valid)
		return -1;

	Item* item = getItem();
	if (item)
		return item->m_itemID;

	return 0;
	*/
}

int ItemInstance::getIdAux() const
{
	//if (!m_item)
	if (m_itemID <= 0)
		return 0;

	return getItem()->buildIdAux(m_auxValue, m_userData);
}

Item* ItemInstance::getItem() const
{
	return Item::items[m_itemID];
}

void ItemInstance::setUserData(CompoundTag* tag)
{
	if (m_userData != tag)
	{
		if (m_userData)
		{
			delete m_userData;
		}

		m_userData = tag;
	}
}

ItemInstance* ItemInstance::copy() const
{
	return new ItemInstance(m_itemID, m_count, m_auxValue);
}

void ItemInstance::set(int inCount)
{
	assert(inCount >= 0);
	if (inCount <= getMaxStackSize())
		assert(!"stack too big!");

	m_count = inCount;

	if (inCount == 0)
		setNull();
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
		m_count--;
		if (m_count < 0)
			m_count = 0;
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

void ItemInstance::mineBlock(const TilePos& pos, Facing::Name face)
{
	return getItem()->mineBlock(this, pos, face);
}

ItemInstance ItemInstance::remove(int count)
{
	m_count -= count;
	return ItemInstance(m_itemID, count, m_auxValue);
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
	ss << m_count << "x" << getItem()->getDescriptionId() << "@" << m_auxValue;

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

int ItemInstance::getAttackDamage(Entity* pEnt)
{
	return getItem()->getAttackDamage(pEnt);
}

bool ItemInstance::isNull() const
{
	// 0.9.2
	if (m_itemID <= 0) // m_field_10, assuming this is m_itemID
		return true;

	if (m_auxValue != 0 ||
		m_count    != 0)
	{
		return false;
	}

	return true; // isNull
}

bool ItemInstance::isNull(const ItemInstance* item)
{
	return item == nullptr || item->isNull();
}

void ItemInstance::setNull()
{
	m_count = 0;
	m_auxValue = 0;
	//m_item = nullptr;
	m_itemID = 0;
	m_popTime = 0;
	if (m_userData)
		delete m_userData;
	m_userData = nullptr;
}

void ItemInstance::load(const CompoundTag& tag)
{
	m_itemID = tag.getInt16("id");
	m_count = tag.getInt8("Count");
	m_auxValue = tag.getInt16("Damage");

	CompoundTag* newTag = nullptr;

	if (tag.contains("tag"))
	{
		newTag = tag.getCompound("tag")->copy();
	}

	m_userData = newTag;
}

CompoundTag& ItemInstance::save(CompoundTag& tag) const
{
	tag.putInt16("id", m_itemID);
	tag.putInt8("Count", m_count);
	tag.putInt16("Damage", getDamageValue());

	if (hasUserData())
		tag.putCompound("tag", m_userData->copy());

	return tag;
}

bool ItemInstance::matches(const ItemInstance* a1, const ItemInstance* a2)
{
	if (a1 == a2 && a1 == nullptr)
		return true;

	if (a1 == nullptr || a2 == nullptr)
		return false;

	return a1 == a2;
}

ItemInstance* ItemInstance::fromTag(const CompoundTag& tag)
{
	ItemInstance* item = new ItemInstance();
	item->load(tag);

	if (!Item::items[item->m_itemID])
	{
		delete item;
		item = nullptr;
	}

	return item;
}

bool ItemInstance::operator==(const ItemInstance& other) const
{
	return this->getAuxValue() == other.getAuxValue() &&
		   this->m_count       == other.m_count &&
		   this->m_itemID      == other.m_itemID;
}

bool ItemInstance::operator!=(const ItemInstance& other) const
{
	// doing this is likely more efficient than inverting the result of == after the fact
	return this->getAuxValue() != other.getAuxValue() ||
		   this->m_count       != other.m_count ||
		   this->m_itemID      != other.m_itemID;
}

/*ItemInstance::operator bool() const
{
	bool result = false;
	if (m_valid)
	{
		Item* item = getItem();
		if (item)
		{
			if (!isNull())
			{
				return true;
			}
		}
	}

	return result;
}*/