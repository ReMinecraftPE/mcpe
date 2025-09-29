#include "Inventory.hpp"
#include "Item.hpp"
#include "nbt/CompoundTag.hpp"

Inventory::Inventory(Player* pPlayer)
{
	m_pPlayer = pPlayer;
	m_selectedHotbarSlot = 0;

	for (int i = 0; i < C_MAX_HOTBAR_ITEMS; i++)
		m_hotbar[i] = -1;
}

Inventory::~Inventory()
{
	for (std::vector<ItemInstance*>::iterator it = m_items.begin(); it != m_items.end(); it++)
	{
		ItemInstance* item = *it;
		delete item;
	}
}

void Inventory::prepareCreativeInventory()
{
	clear();

	// When we've got a proper creative inventory, use this method for aux tiles/items
	//for (int i = 0; i < 16; i++) // <-- This is an example for all wool colors in order

	// Original list of items.
	addCreativeItem(Tile::rock->m_ID);
	addCreativeItem(Tile::stoneBrick->m_ID);
	addCreativeItem(Tile::sandStone->m_ID);
	addCreativeItem(Tile::wood->m_ID);
	addCreativeItem(Tile::treeTrunk->m_ID, 0);
	addCreativeItem(Tile::goldBlock->m_ID);
	addCreativeItem(Tile::ironBlock->m_ID);
	addCreativeItem(Tile::emeraldBlock->m_ID);
	addCreativeItem(Tile::redBrick->m_ID);
	addCreativeItem(Tile::leaves->m_ID, 0);
	addCreativeItem(Tile::cloth->m_ID, 14);
	addCreativeItem(Tile::cloth->m_ID, 13);
	addCreativeItem(Tile::cloth->m_ID, 12);
	addCreativeItem(Tile::cloth->m_ID, 11);
	addCreativeItem(Tile::cloth->m_ID, 10);
	addCreativeItem(Tile::cloth->m_ID, 9);
	addCreativeItem(Tile::cloth->m_ID, 8);
	addCreativeItem(Tile::glass->m_ID);
	addCreativeItem(Tile::cloth->m_ID, 7);
	addCreativeItem(Tile::cloth->m_ID, 6);
	addCreativeItem(Tile::cloth->m_ID, 5);
	addCreativeItem(Tile::cloth->m_ID, 4);
	addCreativeItem(Tile::cloth->m_ID, 3);
	addCreativeItem(Tile::stairs_wood->m_ID);
	addCreativeItem(Tile::stairs_stone->m_ID);
	addCreativeItem(Tile::stoneSlabHalf->m_ID, 0);
	addCreativeItem(Tile::sand->m_ID);
	addCreativeItem(Tile::ladder->m_ID);
	addCreativeItem(Tile::torch->m_ID);
	addCreativeItem(Tile::flower->m_ID);
	addCreativeItem(Tile::rose->m_ID);
	addCreativeItem(Tile::mushroom1->m_ID);
	addCreativeItem(Tile::mushroom2->m_ID);
	addCreativeItem(Tile::reeds->m_ID);
	addCreativeItem(Tile::obsidian->m_ID);
	addCreativeItem(Tile::dirt->m_ID);

	// New items that weren't in the inventory before.
	addCreativeItem(Tile::grass->m_ID);
	addCreativeItem(Tile::tnt->m_ID);
	addCreativeItem(Tile::gravel->m_ID);
	addCreativeItem(Tile::cloth->m_ID, 15);
	addCreativeItem(Tile::mossStone->m_ID);
	addCreativeItem(Tile::bookshelf->m_ID);
	addCreativeItem(Tile::lapisBlock->m_ID);
	addCreativeItem(Tile::sponge->m_ID);
	addCreativeItem(Tile::sapling->m_ID);
	addCreativeItem(Tile::cryingObsidian->m_ID);
	addCreativeItem(Tile::rocketLauncher->m_ID);
	addCreativeItem(Tile::redStoneOre->m_ID);
	
	// test stuff
	addCreativeItem(Tile::water->m_ID);
	addCreativeItem(Tile::lava->m_ID);
	addCreativeItem(Tile::fire->m_ID);

	// items
	addCreativeItem(Item::camera->m_itemID);
	addCreativeItem(Item::door_wood->m_itemID);
	addCreativeItem(Item::door_iron->m_itemID);
	addCreativeItem(Item::rocket->m_itemID);

	// more stuff
	addCreativeItem(Tile::cloth->m_ID, 0);
	addCreativeItem(Tile::cloth->m_ID, 1);
	addCreativeItem(Tile::cloth->m_ID, 2);
	addCreativeItem(Tile::stoneSlabHalf->m_ID, 1);
	addCreativeItem(Tile::stoneSlabHalf->m_ID, 2);
	addCreativeItem(Tile::stoneSlabHalf->m_ID, 3);
	addCreativeItem(Tile::treeTrunk->m_ID, 1);
	addCreativeItem(Tile::treeTrunk->m_ID, 2);
	addCreativeItem(Tile::cactus->m_ID);
	addCreativeItem(Tile::deadBush->m_ID);
	addCreativeItem(Tile::pumpkin->m_ID);
	addCreativeItem(Tile::pumpkinLantern->m_ID);
	addCreativeItem(Tile::netherrack->m_ID);
	addCreativeItem(Tile::soulSand->m_ID);
	addCreativeItem(Tile::glowstone->m_ID);
	addCreativeItem(Tile::web->m_ID);
	addCreativeItem(Tile::fence->m_ID);

	for (int i = 0; i < C_MAX_HOTBAR_ITEMS; i++)
		m_hotbar[i] = i;
}

void Inventory::prepareSurvivalInventory()
{
	clear();
	m_items.resize(C_NUM_SURVIVAL_SLOTS);

	// Add some items for testing
	addTestItem(Item::stick->m_itemID, 64);
	addTestItem(Item::wheat->m_itemID, 64);
	addTestItem(Item::sugar->m_itemID, 64);
	addTestItem(Item::camera->m_itemID, 64);
	addTestItem(Tile::ladder->m_ID, 64);
	addTestItem(Tile::obsidian->m_ID, 64);
	addTestItem(Tile::fire->m_ID, 64);

	for (int i = 0; i < C_MAX_HOTBAR_ITEMS; i++)
		m_hotbar[i] = i;
}

int Inventory::getNumSlots()
{
	switch (_getGameMode())
	{
	case GAME_TYPE_SURVIVAL:
		return C_NUM_SURVIVAL_SLOTS;
	default:
		return getNumItems();
	}
}

int Inventory::getNumItems()
{
	return int(m_items.size());
}

void Inventory::addCreativeItem(int itemID, int auxValue)
{
	m_items.push_back(new ItemInstance(itemID, 1, auxValue));
}

void Inventory::empty()
{
	for (int i = 0; i < m_items.size(); i++)
	{
		delete m_items[i];
		m_items[i] = nullptr;
	}
}

void Inventory::clear()
{
	for (int i = 0; i < m_items.size(); i++)
	{
		delete m_items[i];
	}
	m_items.clear();
}

// This code, and this function, don't exist in b1.2_02
// "add" exists with these same arguments, which calls "addResource",
// but addResource's code is entirely different somehow. Did we write this from scratch?
bool Inventory::addItem(ItemInstance& instance)
{
	if (_getGameMode() == GAME_TYPE_CREATIVE)
	{
		// Just get rid of the item.
		instance.m_count = 0;
		return true;
	}
	
	// look for an item with the same ID
	for (int i = 0; i < getNumItems(); i++)
	{
		ItemInstance* item = m_items[i];
		if (!item || item->m_itemID != instance.m_itemID)
			continue;

		int maxStackSize = item->getMaxStackSize();
		bool bIsStackedByData = instance.getItem()->isStackedByData();
		if (bIsStackedByData && item->getAuxValue() != instance.getAuxValue())
			continue;

		// try to collate.
		int combinedItemAmount = instance.m_count + item->m_count;

		int leftover = combinedItemAmount - maxStackSize;
		if (leftover < 0)
			leftover = 0;
		else
			combinedItemAmount = C_MAX_AMOUNT;

		item->m_count = combinedItemAmount;
		item->m_popTime = 5;

		instance.m_count = leftover;

		if (!bIsStackedByData)
			item->setAuxValue(0);
	}

	// If there's nothing leftover:
	if (instance.m_count <= 0)
		return true;

	// try to add it to an empty slot
	for (int i = 0; i < getNumItems(); i++)
	{
		ItemInstance* item = m_items[i];

		if (item)
		{
			if (item->m_itemID != 0)
				continue;
			delete item;
		}

		item = m_items[i] = new ItemInstance(instance);
        item->m_popTime = 5;
		instance.m_count = 0;
		return true;
	}

	return false;
}

// Doesn't exist in PE
void Inventory::tick()
{
    for (int i = 0; i < m_items.size(); i++)
    {
		ItemInstance* item = m_items[i];

        if (!ItemInstance::isNull(item) && item->m_popTime > 0)
        {
			item->m_popTime--;
        }
    }
}

void Inventory::addTestItem(int itemID, int amount, int auxValue)
{
	ItemInstance inst(itemID, amount, auxValue);
	addItem(inst);

	if (inst.m_count != 0)
	{
		LOG_I("AddTestItem: Couldn't add all %d of %s, only gave %d",
			amount, Item::items[itemID]->m_DescriptionID.c_str(), amount - inst.m_count);
	}
}

ItemInstance* Inventory::getItem(int slotNo)
{
	if (slotNo < 0 || slotNo >= int(m_items.size()))
		return nullptr;

	ItemInstance* item = m_items[slotNo];
	if (!item)
		return nullptr;

	if (item->m_count <= 0)
		item->m_itemID = 0;

	return item;
}

int Inventory::getQuickSlotItemId(int slotNo)
{
	ItemInstance* pInst = getQuickSlotItem(slotNo);
	if (!pInst)
		return -1;

	return pInst->m_itemID;
}

ItemInstance* Inventory::getQuickSlotItem(int slotNo)
{
	if (slotNo < 0 || slotNo >= C_MAX_HOTBAR_ITEMS)
		return nullptr;
	
	ItemInstance* pInst = getItem(m_hotbar[slotNo]);

	return !ItemInstance::isNull(pInst) ? pInst : nullptr;
}

ItemInstance* Inventory::getSelectedItem()
{
	return getQuickSlotItem(m_selectedHotbarSlot);
}

int Inventory::getSelectedItemId()
{
	return getQuickSlotItemId(m_selectedHotbarSlot);
}

void Inventory::selectItem(int slotNo, int maxHotBarSlot)
{
	if (slotNo < 0 || slotNo >= getNumItems())
		return;

	// look for it in the hotbar
	for (int i = 0; i < maxHotBarSlot; i++)
	{
		if (m_hotbar[i] == slotNo)
		{
			m_selectedHotbarSlot = i;
			return;
		}
	}

	for (int i = maxHotBarSlot - 2; i >= 0; i--)
		m_hotbar[i + 1] = m_hotbar[i];

	m_hotbar[0] = slotNo;
	m_selectedHotbarSlot = 0;
}

void Inventory::selectSlot(int slotNo)
{
	if (slotNo < 0 || slotNo >= C_MAX_HOTBAR_ITEMS)
		return;

	m_selectedHotbarSlot = slotNo;
}

void Inventory::setQuickSlotIndexByItemId(int slotNo, int itemID)
{
	if (slotNo < 0 || slotNo >= C_MAX_HOTBAR_ITEMS)
		return;

	if (_getGameMode() == GAME_TYPE_SURVIVAL)
		return; // TODO

	for (int i = 0; i < getNumItems(); i++)
	{
		ItemInstance* item = m_items[i];
		if (item && item->m_itemID == itemID)
		{
			m_hotbar[slotNo] = i;
			return;
		}
	}

	m_hotbar[slotNo] = -1;
}

void Inventory::selectItemById(int itemID, int maxHotBarSlot)
{
	for (int i = 0; i < getNumItems(); i++)
	{
		ItemInstance* item = m_items[i];
		if (!item || item->m_itemID != itemID)
			continue;

		selectItem(i, maxHotBarSlot);
		return;
	}

	LOG_W("selectItemById: %d doesn't exist", itemID);
}

void Inventory::selectItemByIdAux(int itemID, int auxValue, int maxHotBarSlot)
{
	for (int i = 0; i < getNumItems(); i++)
	{
		ItemInstance* item = m_items[i];
		if (!item || item->m_itemID != itemID || item->getAuxValue() != auxValue)
			continue;

		selectItem(i, maxHotBarSlot);
		return;
	}

	LOG_W("selectItemByIdAux: %d:%d doesn't exist", itemID, auxValue);
}

int Inventory::getAttackDamage(Entity* pEnt)
{
	ItemInstance* pInst = getSelected();
	if (ItemInstance::isNull(pInst))
		return 1;

	return pInst->getAttackDamage(pEnt);
}

void Inventory::dropAll(bool onlyClearContainer)
{
	for (int i = 0; i < getNumItems(); i++)
	{
		ItemInstance* item = m_items[i];
		if (item && item->m_count > 0)
		{
			if (!onlyClearContainer)
				m_pPlayer->drop(*item, true);
			item->m_count = 0;
		}
	}
}

void Inventory::save(ListTag& tag) const
{
	if (_getGameMode() == GAME_TYPE_CREATIVE)
		return;

	for (int i = 0; i < m_items.size(); i++)
	{
		const ItemInstance* item = m_items[i];

		if (ItemInstance::isNull(item))
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
		item->save(*itemTag);
		tag.add(itemTag);
	}
}

void Inventory::load(const ListTag& tag)
{
	if (_getGameMode() == GAME_TYPE_CREATIVE)
		return;

	clear();
	m_items.resize(C_NUM_SURVIVAL_SLOTS);

	const std::vector<Tag*>& itemTags = tag.rawView();

	for (std::vector<Tag*>::const_iterator it = itemTags.begin(); it != itemTags.end(); it++)
	{
		const CompoundTag* itemTag = (const CompoundTag*)*it;
		int slot = itemTag->getInt8("Slot") & 255;
		ItemInstance* item = ItemInstance::fromTag(*itemTag);
		if (item)
		{
			if (slot >= 0 && slot < m_items.size())
			{
				m_items[slot] = item;
			}

			/*if (slot >= 100 && slot < m_armor.size() + 100)
			{
				m_armor[slot - 100] = item;
			}*/
		}
	}
}

GameType Inventory::_getGameMode() const
{
	return m_pPlayer->getPlayerGameType();
}
