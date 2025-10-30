/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include <sstream>
#include "ItemInstance.hpp"
#include "GameMods.hpp"
#include "world/tile/Tile.hpp"
#include "nbt/CompoundTag.hpp"

const std::string
ItemInstance::TAG_DISPLAY = "display",
ItemInstance::TAG_DISPLAY_NAME = "Name",
ItemInstance::TAG_REPAIR_COST = "RepairCost",
ItemInstance::TAG_ENCHANTS = "ench";

void ItemInstance::_init(int id, int count, int auxValue)
{
	m_count = count;
	m_auxValue = auxValue;
    m_userData = nullptr;
	m_popTime = 0;
	_setItem(id);
}

ItemInstance::ItemInstance()
{
	_init();
}

ItemInstance::ItemInstance(const ItemInstance& other)
{
	_init(other.getId(), other.m_count, other.getAuxValue());

	bool v6 = false;
	if (other.hasUserData())
	{
		setUserData(other.getUserData()->uniqueClone());
	}
}

ItemInstance::ItemInstance(bool isValid)
{
	_init();
	m_bValid = isValid;
}

ItemInstance::ItemInstance(Item* pItem)
{
	_init(pItem->m_itemID, 1);
}

ItemInstance::ItemInstance(Item* pItem, int count)
{
	_init(pItem->m_itemID, count);
}

ItemInstance::ItemInstance(Item* pItem, int count, int auxValue)
{
	_init(pItem->m_itemID, count, auxValue);
}

ItemInstance::ItemInstance(Tile* pTile)
{
	_init(pTile->m_ID, 1);
}

ItemInstance::ItemInstance(Tile* pTile, int count)
{
	_init(pTile->m_ID, count);
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

bool ItemInstance::_setItem(int id)
{
	if (id == -1)
	{
		m_count = -1;
		m_bValid = false;
		m_pItem = nullptr;
		m_pTile = nullptr;
		m_auxValue = -1;
		return false;
	}

	if (id >= C_MAX_ITEMS)
	{
		return _setItem(-1);
	}

	m_pItem = Item::items[id];
	if (m_pItem)
		m_bValid = true;
	
	if (id < C_MAX_TILES)
	{
		m_pTile = Tile::tiles[id];
		if (m_pTile)
			m_bValid = true;
	}
	else
	{
		m_pTile = nullptr;
	}

	if (m_bValid)
		return true;
	else
		return _setItem(-1);
}

int ItemInstance::getId() const
{
	if (!m_bValid)
		return -1;

	const Item* item = getItem();
	if (item)
		return item->m_itemID;

	const Tile* tile = getTile();
	if (tile)
		return tile->m_ID;

	return 0;
}

int ItemInstance::getIdAux() const
{
	if (!m_pItem)
		return 0;

	return getItem()->buildIdAux(m_auxValue, m_userData);
}

CompoundTag* ItemInstance::getNetworkUserData() const
{
	CompoundTag* userData = new CompoundTag();
	CompoundTag::NamedTagMap& tags = m_userData->rawView();
	for (CompoundTag::NamedTagMap::iterator it = tags.begin(); it != tags.end(); it++)
	{
		const std::string& name = it->first;
		const Tag* tag = it->second;
		if (!tag) continue;

		if (name == TAG_REPAIR_COST)
		{
			continue;
		}
		else if (name == TAG_ENCHANTS)
		{
			ListTag* enchants = new ListTag();
			userData->put(name, enchants);
		}
		else
		{
			userData->put(name, tag->copy());
		}
	}
	return userData;
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

bool ItemInstance::hasSameUserData(const ItemInstance& other) const
{
	if (this->isNull() && other.isNull())
		return true;
	else if (this->isNull() || other.isNull())
		return false;

	if (!this->hasUserData() && !other.hasUserData())
		return true;
	else if (!this->hasUserData() || !other.hasUserData())
		return false;

	return this->getUserData() == other.getUserData();
}

ItemInstance* ItemInstance::copy() const
{
	return new ItemInstance(*this);
}

void ItemInstance::set(int inCount)
{
	assert(inCount >= 0);
	if (inCount > getMaxStackSize())
		assert(!"stack too big!");

	m_count = inCount;

#ifndef MOD_POCKET_SURVIVAL
	if (inCount == 0)
		setNull();
#endif
}

bool ItemInstance::canDestroySpecial(Tile* tile)
{
	return getItem()->canDestroySpecial(tile);
}

std::string ItemInstance::getDescriptionId()
{
	return getItem()->getDescriptionId(this);
}

std::string ItemInstance::getHovertextName() const
{
	if (hasCustomHoverName())
		return getHoverName();
	else
		return getItem()->getHovertextName();
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

bool ItemInstance::isDamageableItem() const
{
	return getItem()->getMaxDamage() > 0;
}

bool ItemInstance::isDamaged() const
{
	return m_auxValue > 0;
}

bool ItemInstance::isStackable() const
{
	if (getMaxStackSize() <= 1)
		return false;

	if (isDamageableItem())
		return !isDamaged();

	return true;
}

bool ItemInstance::isStackedByData() const
{
	return getItem()->isStackedByData();
}

void ItemInstance::mineBlock(const TilePos& pos, Facing::Name face)
{
	return getItem()->mineBlock(this, pos, face);
}

void ItemInstance::remove(int count)
{
	set(m_count - count);
}

void ItemInstance::setDescriptionId(const std::string& str)
{
}

void ItemInstance::snap(Player*)
{

}

std::string ItemInstance::toString() const
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

int ItemInstance::getAttackDamage(Entity* pEnt) const
{
	return getItem()->getAttackDamage(pEnt);
}

bool ItemInstance::isNull() const
{
	if (!m_bValid)
		return true;

	if (m_count != 0 ||
		m_pItem || m_pTile ||
		hasUserData())
	{
		return false;
	}

	return true;
}

bool ItemInstance::isNull(const ItemInstance* item)
{
	return item == nullptr || item->isNull();
}

void ItemInstance::setNull()
{
	m_count = 0;
	m_auxValue = 0;
	m_pItem = nullptr;
	m_pTile = nullptr;
	m_popTime = 0;
	if (m_userData)
		delete m_userData;
	m_userData = nullptr;
}

int ItemInstance::getBaseRepairCost() const
{
	if (hasUserData() && m_userData->contains(TAG_REPAIR_COST))
		return m_userData->getInt32(TAG_REPAIR_COST);
	else
		return 0;
}

void ItemInstance::setRepairCost(int repairCost)
{
	if (!hasUserData())
		m_userData = new CompoundTag();

	m_userData->putInt32(ItemInstance::TAG_REPAIR_COST, repairCost);
}

bool ItemInstance::hasCustomHoverName() const
{
	if (!hasUserData() || !m_userData->contains(TAG_DISPLAY))
		return false;

	const CompoundTag* displayTag = m_userData->getCompound(TAG_DISPLAY);
	return displayTag->contains(TAG_DISPLAY_NAME);
}

std::string ItemInstance::getHoverName() const
{
	std::string hoverName;
	if (hasUserData())
	{
		if (m_userData->contains(TAG_DISPLAY))
		{
			const CompoundTag* displayTag = m_userData->getCompound(TAG_DISPLAY);
			if (displayTag->contains(TAG_DISPLAY_NAME))
			{
				hoverName = displayTag->getString(TAG_DISPLAY_NAME);
			}
		}
	}
	return hoverName;
}

void ItemInstance::setHoverName(const std::string& hoverName)
{
	if (!hasUserData())
		m_userData = new CompoundTag();

	CompoundTag* displayTag;
	if (m_userData->contains(TAG_DISPLAY))
	{
		displayTag = m_userData->getCompound(TAG_DISPLAY);
	}
	else
	{
		displayTag = new CompoundTag();
		m_userData->putCompound(TAG_DISPLAY, displayTag);
	}

	CompoundTag* newDisplayTag = displayTag->uniqueClone();
	newDisplayTag->putString(TAG_DISPLAY_NAME, hoverName);

	m_userData->putCompound(TAG_DISPLAY, newDisplayTag);
}

void ItemInstance::load(const CompoundTag& tag)
{
	_setItem(tag.getInt16("id"));
	m_count = tag.getInt8("Count");
	setAuxValue(tag.getInt16("Damage"));

	CompoundTag* userData = nullptr;

	if (tag.contains("tag"))
	{
		userData = tag.getCompound("tag")->copy();
	}

	m_userData = userData;
}

CompoundTag& ItemInstance::save(CompoundTag& tag) const
{
	tag.putInt16("id", getId());
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

	if (!item->getItem())
	{
		delete item;
		item = nullptr;
	}

	return item;
}

ItemInstance* ItemInstance::operator=(const ItemInstance& other)
{
	this->m_count = other.m_count;
	this->m_pItem = other.m_pItem;
	this->m_pTile = other.m_pTile;
	this->m_auxValue = other.m_auxValue;
	this->m_bValid = other.m_bValid;

	if (other.hasUserData())
	{
		this->m_userData = other.m_userData->uniqueClone();
	}
	else
	{
		this->m_userData = nullptr;
	}

	return this;
}

bool ItemInstance::operator==(const ItemInstance& other) const
{
	// 0.12.1 seems to intentionally leave out the auxValue comparison, not sure why
	// but I have a feeling that doing the same will break things
	return this->getItem()     == other.getItem() &&
		   this->getAuxValue() == other.getAuxValue() &&
		   this->m_count       == other.m_count &&
		   this->hasSameUserData(other);
}

bool ItemInstance::operator!=(const ItemInstance& other) const
{
	// doing this is likely more efficient than inverting the result of == after the fact
	return this->getItem()     != other.getItem() ||
		   this->getAuxValue() != other.getAuxValue() ||
		   this->m_count       != other.m_count ||
		   !this->hasSameUserData(other);
}

ItemInstance::operator bool() const
{
	return m_bValid && getItem() && !isNull();
}