/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include <sstream>
#include "ItemStack.hpp"
#include "GameMods.hpp"
#include "world/tile/Tile.hpp"
#include "nbt/CompoundTag.hpp"

const std::string
ItemStack::TAG_DISPLAY = "display",
ItemStack::TAG_DISPLAY_NAME = "Name",
ItemStack::TAG_REPAIR_COST = "RepairCost",
ItemStack::TAG_ENCHANTS = "ench";
const ItemStack ItemStack::EMPTY;

void ItemStack::_init(int id, int count, int auxValue)
{
	m_count = count;
	m_auxValue = auxValue;
    m_userData = nullptr;
	m_popTime = 0;
	m_bValid = false;
	_setItem(id);
}

ItemStack::ItemStack()
{
	_init();
}

ItemStack::ItemStack(const ItemStack& other)
{
	_init(other.getId(), other.m_count, other.getAuxValue());

	if (other.hasUserData())
	{
		setUserData(other.getUserData()->uniqueClone());
	}
}

ItemStack::ItemStack(bool isValid)
{
	_init();
	m_bValid = isValid;
}

ItemStack::ItemStack(Item* pItem)
{
	_init(pItem->m_itemID, 1);
}

ItemStack::ItemStack(Item* pItem, int count)
{
	_init(pItem->m_itemID, count);
}

ItemStack::ItemStack(Item* pItem, int count, int auxValue)
{
	_init(pItem->m_itemID, count, auxValue);
}

ItemStack::ItemStack(Tile* pTile)
{
	_init(pTile->m_ID, 1);
}

ItemStack::ItemStack(Tile* pTile, int count)
{
	_init(pTile->m_ID, count);
}

ItemStack::ItemStack(Tile* pTile, int amount, int auxValue)
{
	_init(pTile->m_ID, amount, auxValue);
}

ItemStack::ItemStack(int itemID, int amount, int auxValue)
{
	_init(itemID, amount, auxValue);
}

ItemStack::~ItemStack()
{
    SAFE_DELETE(m_userData);
}

bool ItemStack::_setItem(int id)
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

int ItemStack::getId() const
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

int ItemStack::getIdAux() const
{
	if (!m_pItem)
		return 0;

	return getItem()->buildIdAux(m_auxValue, m_userData);
}

CompoundTag* ItemStack::getNetworkUserData() const
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

void ItemStack::setUserData(CompoundTag* tag)
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

bool ItemStack::hasSameUserData(const ItemStack& other) const
{
	if (this->isEmpty() && other.isEmpty())
		return true;
	else if (this->isEmpty() || other.isEmpty())
		return false;

	if (!this->hasUserData() && !other.hasUserData())
		return true;
	else if (!this->hasUserData() || !other.hasUserData())
		return false;

	return this->getUserData() == other.getUserData();
}

ItemStack* ItemStack::copy() const
{
	return new ItemStack(*this);
}

void ItemStack::set(int inCount)
{
	assert(inCount >= 0);
	if (inCount > getMaxStackSize())
		assert(!"stack too big!");

	m_count = inCount;

#ifndef MOD_POCKET_SURVIVAL
	if (inCount == 0)
		setEmpty();
#endif
}

bool ItemStack::canDestroySpecial(Tile* tile)
{
	return getItem()->canDestroySpecial(tile);
}

std::string ItemStack::getDescriptionId()
{
	return getItem()->getDescriptionId(this);
}

std::string ItemStack::getHovertextName() const
{
	if (hasCustomHoverName())
		return getHoverName();
	else
		return getItem()->getHovertextName();
}

float ItemStack::getDestroySpeed(Tile* tile)
{
	return getItem()->getDestroySpeed(this, tile);
}

int ItemStack::getIcon() const
{
	return getItem()->getIcon(this);
}

int ItemStack::getMaxDamage() const
{
	return getItem()->getMaxDamage();
}

int ItemStack::getMaxStackSize() const
{
	return getItem()->getMaxStackSize();
}

void ItemStack::hurt(int by)
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

void ItemStack::hurtAndBreak(int amount, Entity* ent)
{
	if (isDamageableItem())
	{
		m_auxValue += amount;
		if (m_auxValue > getMaxDamage())
		{
			if (ent->isPlayer())
			{
				//((Player*)ent)->awardStat(Stats::statItemBreak[m_itemID]);
			}

			--m_count;
			if (m_count < 0)
				m_count = 0;
			m_auxValue = 0;
		}
	}
}

void ItemStack::hurtEnemy(Mob* mob)
{
	getItem()->hurtEnemy(this, mob);
}

void ItemStack::interactEnemy(Mob* mob)
{
	getItem()->interactEnemy(this, mob);
}

bool ItemStack::isDamageableItem() const
{
	return getItem()->getMaxDamage() > 0;
}

bool ItemStack::isDamaged() const
{
	return isDamageableItem() && m_auxValue > 0;
}

bool ItemStack::isStackable() const
{
	if (getMaxStackSize() <= 1)
		return false;

	if (isDamageableItem())
		return !isDamaged();

	return true;
}

bool ItemStack::isStackedByData() const
{
	return getItem()->isStackedByData();
}

void ItemStack::mineBlock(const TilePos& pos, Facing::Name face)
{
	return getItem()->mineBlock(this, pos, face);
}

void ItemStack::shrink(int count)
{
	set(m_count - count);
}

ItemStack ItemStack::remove(int count)
{
	shrink(count);
	return ItemStack(getId(), count, m_auxValue);
}

void ItemStack::setDescriptionId(const std::string& str)
{
}

void ItemStack::snap(Player*)
{

}

std::string ItemStack::toString() const
{
	std::stringstream ss;
	ss << m_count << "x" << getItem()->getDescriptionId() << "@" << m_auxValue;

	return ss.str();
}

ItemStack* ItemStack::use(Level* level, Player* player)
{
	return getItem()->use(this, level, player);
}

bool ItemStack::useOn(Player* player, Level* level, const TilePos& pos, Facing::Name face)
{
	return getItem()->useOn(this, player, level, pos, face);
}

void ItemStack::onCraftedBy(Player* player, Level* level)
{
	onCraftedBy(player, level, m_count);
}

void ItemStack::onCraftedBy(Player* player, Level* level, int amount)
{
	//player->awardStat(Stats::itemCrafted[getId()], amount);
	getItem()->onCraftedBy(this, player, level);
}

int ItemStack::getAttackDamage(Entity* pEnt) const
{
	return getItem()->getAttackDamage(pEnt);
}

bool ItemStack::isEmpty() const
{
	if (!m_bValid)
		return true;

	return m_count <= 0 || !m_pItem;
}

bool ItemStack::isEmpty(const ItemStack* item)
{
	return item == nullptr || item->isEmpty();
}

void ItemStack::setEmpty()
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

int ItemStack::getBaseRepairCost() const
{
	if (hasUserData() && m_userData->contains(TAG_REPAIR_COST))
		return m_userData->getInt32(TAG_REPAIR_COST);
	else
		return 0;
}

void ItemStack::setRepairCost(int repairCost)
{
	if (!hasUserData())
		m_userData = new CompoundTag();

	m_userData->putInt32(ItemStack::TAG_REPAIR_COST, repairCost);
}

bool ItemStack::hasCustomHoverName() const
{
	if (!hasUserData() || !m_userData->contains(TAG_DISPLAY))
		return false;

	const CompoundTag* displayTag = m_userData->getCompound(TAG_DISPLAY);
	return displayTag->contains(TAG_DISPLAY_NAME);
}

std::string ItemStack::getHoverName() const
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

void ItemStack::setHoverName(const std::string& hoverName)
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

void ItemStack::load(const CompoundTag& tag)
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

CompoundTag& ItemStack::save(CompoundTag& tag) const
{
	tag.putInt16("id", getId());
	tag.putInt8("Count", m_count);
	tag.putInt16("Damage", getDamageValue());

	if (hasUserData())
		tag.putCompound("tag", m_userData->copy());

	return tag;
}

bool ItemStack::matches(const ItemStack& a1, const ItemStack& a2)
{
	if (&a1 == &a2 && a1.isEmpty())
		return true;

	if (a1.isEmpty() || a2.isEmpty())
		return false;

	return a1 == a2;
}

ItemStack ItemStack::fromTag(const CompoundTag& tag)
{
	ItemStack item;
	item.load(tag);
	return item;
}

ItemStack* ItemStack::operator=(const ItemStack& other)
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

bool ItemStack::operator==(const ItemStack& other) const
{
	// 0.12.1 seems to intentionally leave out the auxValue comparison, not sure why
	// but I have a feeling that doing the same will break things
	return this->getItem()     == other.getItem() &&
		   this->getAuxValue() == other.getAuxValue() &&
		   this->m_count       == other.m_count &&
		   this->hasSameUserData(other);
}

bool ItemStack::operator!=(const ItemStack& other) const
{
	// doing this is likely more efficient than inverting the result of == after the fact
	return this->getItem()     != other.getItem() ||
		   this->getAuxValue() != other.getAuxValue() ||
		   this->m_count       != other.m_count ||
		   !this->hasSameUserData(other);
}

ItemStack::operator bool() const
{
	return m_bValid && getItem() && !isEmpty();
}
