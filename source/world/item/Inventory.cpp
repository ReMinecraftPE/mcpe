#include "Inventory.hpp"

#include "GameMods.hpp"
#include "common/Logger.hpp"
#include "nbt/CompoundTag.hpp"

#include "Item.hpp"

Inventory::Inventory(Player* pPlayer) : m_items(C_NUM_INVENTORY_SLOTS), m_armor(C_NUM_ARMOR_SLOTS)
{
	m_pPlayer = pPlayer;
	m_selectedSlot = 0;
}

Inventory::~Inventory()
{
}

void Inventory::prepareCreativeInventory()
{
	addCreativeItem(Tile::stone->m_ID);
	addCreativeItem(Tile::cobblestone->m_ID);
	addCreativeItem(Tile::brick->m_ID);
	addCreativeItem(Tile::dirt->m_ID);
	addCreativeItem(Tile::wood->m_ID);
	addCreativeItem(Tile::treeTrunk->m_ID);
	addCreativeItem(Tile::leaves->m_ID);
	addCreativeItem(Tile::torch->m_ID);
	addCreativeItem(Tile::stoneSlabHalf->m_ID);
}

void Inventory::prepareSurvivalInventory()
{
	// Add some items for testing
	/*addTestItem(Item::stick->m_itemID, 64);
	addTestItem(Item::wheat->m_itemID, 64);
	addTestItem(Item::sugar->m_itemID, 64);
	addTestItem(Item::camera->m_itemID, 64);
	addTestItem(Tile::ladder->m_ID, 64);
	addTestItem(Tile::obsidian->m_ID, 64);
	addTestItem(Tile::fire->m_ID, 64);*/

	// 0.2.1 items
#ifdef MOD_POCKET_SURVIVAL
	addCreativeItem(ITEM_SHOVEL_STONE);
	addCreativeItem(ITEM_PICKAXE_STONE);
	addCreativeItem(ITEM_HATCHET_STONE);
	addCreativeItem(ITEM_SHEARS);
	addCreativeItem(ITEM_SWORD_STONE);
	addCreativeItem(TILE_LADDER);
	addCreativeItem(TILE_TORCH);
	addCreativeItem(ITEM_DOOR_WOOD);
	addCreativeItem(TILE_FENCE);
	//addCreativeItem(TILE_FENCEGATE);
	addCreativeItem(TILE_COBBLESTONE);
	addCreativeItem(TILE_TREE_TRUNK, 1);
	addCreativeItem(TILE_TREE_TRUNK, 2);
	addCreativeItem(TILE_WOOD);
	addCreativeItem(TILE_DIRT);
	addCreativeItem(TILE_SANDSTONE);
	addCreativeItem(TILE_GRAVEL);
	addCreativeItem(TILE_STONE);
	addCreativeItem(TILE_STAIRS_WOOD);
	addCreativeItem(TILE_STAIRS_STONE);
	addCreativeItem(TILE_STONESLAB_HALF);
	addCreativeItem(TILE_SAND);
	addCreativeItem(TILE_CLOTH, 7);
	addCreativeItem(TILE_CLOTH, 6);
	addCreativeItem(TILE_CLOTH, 5);
	addCreativeItem(TILE_CLOTH, 4);
	addCreativeItem(TILE_CLOTH, 3);
	addCreativeItem(TILE_CLOTH, 15);
	addCreativeItem(TILE_CLOTH, 14);
	addCreativeItem(TILE_CLOTH, 13);
	addCreativeItem(TILE_CLOTH, 12);
	addCreativeItem(TILE_CLOTH, 11);
	addCreativeItem(TILE_CLOTH, 10);
	addCreativeItem(TILE_CLOTH, 9);
	addCreativeItem(TILE_CLOTH, 8);
	addCreativeItem(TILE_GLASS);
	addCreativeItem(TILE_LEAVES);

	if (_getGameMode() == GAME_TYPE_CREATIVE)
	{
		addCreativeItem(TILE_BLOCK_GOLD);
		addCreativeItem(TILE_BLOCK_IRON);
		addCreativeItem(TILE_BLOCK_DIAMOND);
		addCreativeItem(TILE_OBSIDIAN);
		addCreativeItem(TILE_BOOKSHELF);
	}
	else
	{
		addCreativeItem(TILE_OBSIDIAN); // count of 0
	}

	addCreativeItem(TILE_FLOWER);
	addCreativeItem(TILE_ROSE);
	addCreativeItem(TILE_MUSHROOM_1);
	addCreativeItem(TILE_MUSHROOM_2);
	addCreativeItem(TILE_CACTUS);
	addCreativeItem(ITEM_REEDS);

	for (size_t i = 0; i < m_items.size(); i++)
	{
		ItemStack& item = m_items[i];
		if (_getGameMode() == GAME_TYPE_SURVIVAL && !hasUnlimitedResource(item))
		{
			item.m_count = 0;
		}
	}
#endif
}

uint16_t Inventory::getContainerSize() const
{
	return uint16_t(m_items.size() + m_armor.size());
}

void Inventory::clear()
{
	std::fill(m_items.begin(), m_items.end(), ItemStack::EMPTY);
	std::fill(m_armor.begin(), m_armor.end(), ItemStack::EMPTY);
}

void Inventory::addCreativeItem(int itemID, int auxValue)
{
	ItemStack item(itemID, 1, auxValue);
	add(item);
}

bool Inventory::add(ItemStack& item)
{
	if (!item.isDamaged())
	{
		int oldCount;
		do
		{
			oldCount = item.m_count;
			item.m_count = addResource(item);
		}
		while (item.m_count > 0 && item.m_count < oldCount);

		return item.m_count < oldCount;
	}

	int freeSlot = getFreeSlot();
	if (freeSlot >= 0)
	{
		m_items[freeSlot] = item;
		m_items[freeSlot].m_popTime = C_POP_TIME_DURATION;
		item.m_count = 0;
		return true;
	}
	else
		return false;
}

bool Inventory::contains(const ItemStack& item) const
{
	for (std::vector<ItemStack>::const_iterator it = m_armor.begin(); it != m_armor.end(); ++it)
	{
		if ((*it) == item)
			return true;
	}

	for (std::vector<ItemStack>::const_iterator it = m_items.begin(); it != m_items.end(); ++it)
	{
		if ((*it)  == item)
			return true;
	}

	return false;
}

int Inventory::getSlotWithRemainingSpace(const ItemStack& item)
{
	for (size_t index = 0; index < m_items.size(); ++index)
	{
		ItemStack& i = m_items[index];
		if (!i.isEmpty() && i.getId() == item.getId() && i.isStackable() && i.m_count < i.getMaxStackSize() && i.m_count < getMaxStackSize() && (!i.isStackedByData() || i.getAuxValue() == item.getAuxValue()))
			return index;
	}

	return -1;
}

int Inventory::getFreeSlot()
{
	for (size_t i = 0; i < m_items.size(); ++i)
	{
		if (m_items[i].isEmpty())
			return i;
	}

	return -1;
}

int Inventory::addResource(const ItemStack& item)
{
	int id = item.getId();
	int count = item.m_count;
	int slot = getSlotWithRemainingSpace(item);

	if (slot < 0) slot = getFreeSlot();

	if (slot < 0)
		return count;
	else
	{
		if (m_items[slot].isEmpty())
			m_items[slot] = ItemStack(id, 0, item.getAuxValue());

		int oldCount = count;
		if (count > m_items[slot].getMaxStackSize() - m_items[slot].m_count)
			oldCount = m_items[slot].getMaxStackSize() - m_items[slot].m_count;


		if (oldCount > getMaxStackSize() - m_items[slot].m_count)
			oldCount = getMaxStackSize() - m_items[slot].m_count;

		if (!oldCount)
		{
			return count;
		}
		else
		{
			count -= oldCount;
			m_items[slot].m_count += oldCount;
			m_items[slot].m_popTime = C_POP_TIME_DURATION;
			return count;
		}
	}
}

ItemStack Inventory::removeItem(int index, int count)
{
	ItemStack& item = getItem(index);

	if (!item.isEmpty())
	{
		if (item.m_count <= count)
		{
			ItemStack removed = item;
			setItem(index, ItemStack::EMPTY);
			return removed;
		}
		else
		{
			ItemStack removed = item.remove(count);
			if (item.m_count == 0)
				setItem(index, ItemStack::EMPTY);

			return removed;
		}
	}
	else
		return ItemStack::EMPTY;
}

int Inventory::getSlot(int id)
{
	for (size_t i = 0; i < m_items.size(); ++i)
	{
		if (!m_items[i].isEmpty() && m_items[i].getId() == id)
			return i;
	}

	return -1;
}

bool Inventory::removeResource(int id)
{
	int slot = getSlot(id);
	if (slot < 0)
		return false;
	else
	{
		if (--m_items[slot].m_count <= 0)
			m_items[slot] = ItemStack::EMPTY;

		return true;
	}
}

// Doesn't exist in PE
void Inventory::tick()
{
    for (size_t i = 0; i < m_items.size(); i++)
    {
		ItemStack& item = m_items[i];

        if (!item.isEmpty())
        {
			if (item.m_popTime > 0)
				item.m_popTime--;
			item.getItem()->inventoryTick(&item, m_pPlayer->m_pLevel, m_pPlayer, i, i == m_selectedSlot);
        }
    }
}

void Inventory::addTestItem(int itemID, int amount, int auxValue)
{
	ItemStack item(itemID, amount, auxValue);
	add(item);

	if (item.m_count != 0)
	{
		LOG_I("AddTestItem: Couldn't add all %d of %s, only gave %d",
			amount, Item::items[itemID]->m_DescriptionID.c_str(), amount - item.m_count);
	}
}

bool Inventory::hasUnlimitedResource(const ItemStack& item) const
{
	if (item.isEmpty())
		return false;

	int itemId = item.getId();

	switch (itemId)
	{
	case ITEM_DOOR_WOOD:
		return true;
	}

	// strictly an item, not a tile
	if (!item.getTile())
		return true;

	// big ol' if statement in 0.2.1
	switch (itemId)
	{
	case TILE_DIRT:
	case TILE_GRAVEL:
	case TILE_STONE:
	case TILE_SAND:
	case TILE_SANDSTONE:
	case TILE_CACTUS:
	case TILE_TREE_TRUNK:

	case TILE_MUSHROOM_1:
	case TILE_MUSHROOM_2:
	case TILE_FLOWER:
	case TILE_ROSE:
	case TILE_COBBLESTONE:

	case TILE_OBSIDIAN:
		return false;

	case TILE_LEAVES:
		return true;
	}

	return true;
}

ItemStack& Inventory::getItem(int slotNo)
{
	assert(slotNo >= 0 && slotNo < getContainerSize());

	if (size_t(slotNo) < m_items.size())
		return m_items[slotNo];
	else
		return m_armor[slotNo - m_items.size()];
}

ItemStack& Inventory::getArmor(Item::EquipmentSlot slotNo)
{
	return m_armor[slotNo];
}

ItemStack& Inventory::getSelectedItem()
{
	return getItem(m_selectedSlot);
}

int Inventory::getSelectedItemId()
{
	return getItem(m_selectedSlot).getId();
}

void Inventory::setItem(int index, const ItemStack& item)
{
	if ((size_t)index >= m_items.size())
	{
		m_armor[index - m_items.size()] = item;
	}
	else
	{
		m_items[index] = item;
	}
}

void Inventory::setSelectedItem(ItemStack item)
{
	setItem(m_selectedSlot, item);
}

void Inventory::setCarried(const ItemStack& carried)
{
	m_carried = carried;
	//m_pPlayer->carriedChanged(carried);
}

ItemStack& Inventory::getCarried()
{
	return m_carried;
}

void Inventory::pickItem(int itemID, int data, int maxHotBarSlot)
{
	Item* selectItem = Item::items[itemID];

	if (!selectItem) return;

	for (size_t i = 0; i < m_items.size(); i++)
	{
		if (!m_items[i] || m_items[i].getId() != itemID || m_items[i].getAuxValue() != data)
			continue;

		if (i < size_t(maxHotBarSlot))
			selectSlot(i);
		else
			swapItems(i, m_selectedSlot);
		return;
	}

	if (m_pPlayer->isCreative())
	{
		ItemStack oldSelected = getSelected();
		setItem(m_selectedSlot, ItemStack(selectItem, 1, data));
		if (!oldSelected.isEmpty()) addResource(oldSelected);
	}
}

void Inventory::selectItem(int itemID, int maxHotBarSlot)
{
	Item* selectItem = Item::items[itemID];

	if (!selectItem) return;

	for (size_t i = 0; i < m_items.size(); i++)
	{
		if (!m_items[i] || m_items[i].getId() != itemID)
			continue;

		if (i < size_t(maxHotBarSlot))
			m_selectedSlot = i;
		else
			swapItems(i, m_selectedSlot);
	}
}

void Inventory::swapItems(int indexA, int indexB)
{
	std::swap(getItem(indexA), getItem(indexB));
}

void Inventory::selectSlot(int slotNo)
{
	if (slotNo < 0 || slotNo >= C_MAX_HOTBAR_ITEMS)
		return;

	m_selectedSlot = slotNo;
}

int Inventory::getAttackDamage(Entity* pEnt)
{
	ItemStack& item = getSelected();
	if (item.isEmpty())
		return 1;

	return item.getAttackDamage(pEnt);
}

int Inventory::getArmorValue() const
{
	int allDefense = 0;
	int allDurability = 0;
	int allMaxDamage = 0;

	for (std::vector<ItemStack>::const_iterator it = m_armor.begin(); it != m_armor.end(); ++it)
	{
		const ItemStack& armor = (*it);

		if (armor.isEmpty() || armor.getItem()->getDefense() <= 0)
			continue;

		int maxDamage = armor.getMaxDamage();
		int damageValue = armor.getDamageValue();
		int durability = maxDamage - damageValue;
		allDurability += durability;
		allMaxDamage += maxDamage;
		allDefense += armor.getItem()->getDefense();
	}

	if (allMaxDamage == 0)
		return 0;
	else
		return (allDefense - 1) * allDurability / allMaxDamage + 1;
}

void Inventory::hurtArmor(int amount)
{
	for (size_t i = 0; i < m_armor.size(); i++)
	{
		ItemStack& armor = m_armor[i];

		if (!armor || armor.getItem()->getDefense() <= 0)
			continue;

		armor.hurtAndBreak(amount, m_pPlayer);
		if (!armor.m_count)
		{
			armor.snap(m_pPlayer);
			m_armor[i] = ItemStack::EMPTY;
		}
	}
}

void Inventory::dropAll(bool onlyClearContainer)
{
	for (int i = 0; i < getContainerSize(); i++)
	{
		ItemStack& item = getItem(i);
		if (!item.isEmpty())
		{
			if (!onlyClearContainer)
				m_pPlayer->drop(item, true);
			item = ItemStack::EMPTY;
		}
	}
}

void Inventory::save(ListTag& tag) const
{
	if (_getGameMode() == GAME_TYPE_CREATIVE)
		return;

	for (size_t i = 0; i < m_items.size(); i++)
	{
		const ItemStack& item = m_items[i];

		if (item.isEmpty())
			continue;

		CompoundTag* itemTag = new CompoundTag();
		itemTag->putInt8("Slot", i);
		/* On PE, Mojang for some reason limited something saved as a 16-bit signed integer to a 0-255 range.
		if (item.getAuxValue() < 0)
		{
			item.setAuxValue(0);
		}
		else if (item.getAuxValue > 255)
		{
			item.setAuxValue(255);
		}
		*/
		item.save(*itemTag);
		tag.add(itemTag);
	}

	for (size_t i = 0; i < m_armor.size(); i++)
	{
		const ItemStack& item = m_armor[i];

		if (item.isEmpty())
			continue;

		CompoundTag* itemTag = new CompoundTag();
		itemTag->putInt8("Slot", i + 100);
		item.save(*itemTag);
		tag.add(itemTag);
	}
}

void Inventory::load(const ListTag& tag)
{
	if (_getGameMode() == GAME_TYPE_CREATIVE)
		return;

	const std::vector<Tag*>& itemTags = tag.rawView();

	for (std::vector<Tag*>::const_iterator it = itemTags.begin(); it != itemTags.end(); it++)
	{
		const CompoundTag* itemTag = (const CompoundTag*)*it;
		uint8_t slot = itemTag->getInt8("Slot") & 255;
		ItemStack item = ItemStack::fromTag(*itemTag);
		if (!item.isEmpty())
		{
			if (slot >= 0 && slot < m_items.size())
			{
				m_items[slot] = item;

#ifdef MOD_POCKET_SURVIVAL
				// 0.2.1
				if (m_items[slot].m_count == 0 && hasUnlimitedResource(item))
				{
					m_items[slot].m_count = 1;
				}
#endif
			}

			if (slot >= 100 && slot < m_armor.size() + 100)
			{
				m_armor[slot - 100] = item;
			}
		}
	}
}

GameType Inventory::_getGameMode() const
{
	return m_pPlayer->getPlayerGameType();
}
