#include "Inventory.hpp"
#include "Item.hpp"

Inventory::Inventory(Player* pPlayer)
{
	m_pPlayer = pPlayer;
	m_selectedHotbarSlot = 0;
	m_bIsSurvival = false;

	for (int i = 0; i < C_MAX_HOTBAR_ITEMS; i++)
		m_hotbar[i] = -1;
}

void Inventory::prepareCreativeInventory()
{
	m_bIsSurvival = false;

	m_items.clear();

	// Original list of items.
	addCreativeItem(Tile::rock->m_ID);
	addCreativeItem(Tile::stoneBrick->m_ID);
	addCreativeItem(Tile::sandStone->m_ID);
	addCreativeItem(Tile::wood->m_ID);
	addCreativeItem(Tile::treeTrunk->m_ID);
	addCreativeItem(Tile::goldBlock->m_ID);
	addCreativeItem(Tile::ironBlock->m_ID);
	addCreativeItem(Tile::emeraldBlock->m_ID);
	addCreativeItem(Tile::redBrick->m_ID);
	addCreativeItem(Tile::leaves->m_ID);
	addCreativeItem(Tile::cloth_10->m_ID);
	addCreativeItem(Tile::cloth_20->m_ID);
	addCreativeItem(Tile::cloth_30->m_ID);
	addCreativeItem(Tile::cloth_40->m_ID);
	addCreativeItem(Tile::cloth_50->m_ID);
	addCreativeItem(Tile::cloth_60->m_ID);
	addCreativeItem(Tile::cloth_70->m_ID);
	addCreativeItem(Tile::glass->m_ID);
	addCreativeItem(Tile::cloth_01->m_ID);
	addCreativeItem(Tile::cloth_11->m_ID);
	addCreativeItem(Tile::cloth_21->m_ID);
	addCreativeItem(Tile::cloth_31->m_ID);
	addCreativeItem(Tile::cloth_41->m_ID);
	addCreativeItem(Tile::stairs_wood->m_ID);
	addCreativeItem(Tile::stairs_stone->m_ID);
	addCreativeItem(Tile::stoneSlabHalf->m_ID);
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
	addCreativeItem(Tile::cloth->m_ID);
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
	addCreativeItem(Item::rocket->m_itemID);

	// redstone stuff
	addCreativeItem(Item::door_wood->m_itemID);
	addCreativeItem(Item::door_iron->m_itemID);
	addCreativeItem(Item::redStone->m_itemID);
	addCreativeItem(Tile::notGate->m_ID);
	addCreativeItem(Tile::lever->m_ID);
	addCreativeItem(Tile::button->m_ID);
	addCreativeItem(Tile::plate_stone->m_ID);
	addCreativeItem(Tile::plate_wood->m_ID);
	addCreativeItem(Item::diode->m_itemID);

	for (int i = 0; i < C_MAX_HOTBAR_ITEMS; i++)
		m_hotbar[i] = i;
}

void Inventory::prepareSurvivalInventory()
{
	m_bIsSurvival = true;
	m_items.clear();
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
	if (m_bIsSurvival)
		return C_NUM_SURVIVAL_SLOTS;

	return getNumItems();
}

int Inventory::getNumItems()
{
	return int(m_items.size());
}

void Inventory::addCreativeItem(int itemID, int auxValue)
{
	m_items.push_back(ItemInstance(itemID, 1, auxValue));
}

void Inventory::clear()
{
	m_items.clear();
	m_items.resize(C_NUM_SURVIVAL_SLOTS);
}

void Inventory::addItem(ItemInstance* pInst)
{
	if (!m_bIsSurvival)
	{
		// Just get rid of the item.
		pInst->m_amount = 0;
		return;
	}

	// look for an item with the same ID
	for (int i = 0; i < getNumItems(); i++)
	{
		if (m_items[i].m_itemID != pInst->m_itemID)
			continue;

		int maxStackSize = m_items[i].getMaxStackSize();
		bool bIsStackedByData = Item::items[pInst->m_itemID]->isStackedByData();
		if (bIsStackedByData && m_items[i].m_auxValue != pInst->m_auxValue)
			continue;

		// try to collate.
		int combinedItemAmount = pInst->m_amount + m_items[i].m_amount;

		int leftover = combinedItemAmount - maxStackSize;
		if (leftover < 0)
			leftover = 0;
		else
			combinedItemAmount = C_MAX_AMOUNT;

		m_items[i].m_amount = combinedItemAmount;

		pInst->m_amount = leftover;

		if (!bIsStackedByData)
			m_items[i].m_auxValue = 0;
	}

	// If there's nothing leftover:
	if (!pInst->m_amount)
		return;

	// try to add it to an empty slot
	for (int i = 0; i < getNumItems(); i++)
	{
		if (m_items[i].m_itemID != 0)
			continue;

		m_items[i] = *pInst;
		pInst->m_amount = 0;
		return;
	}
}

void Inventory::addTestItem(int itemID, int amount, int auxValue)
{
	ItemInstance inst(itemID, amount, auxValue);
	addItem(&inst);

	if (inst.m_amount != 0)
	{
		LOG_I("AddTestItem: Couldn't add all %d of %s, only gave %d",
			amount, Item::items[itemID]->m_DescriptionID.c_str(), amount - inst.m_amount);
	}
}

ItemInstance* Inventory::getItem(int slotNo)
{
	if (slotNo < 0 || slotNo >= int(m_items.size()))
		return nullptr;

	if (m_items[slotNo].m_amount <= 0)
		m_items[slotNo].m_itemID = 0;

	return &m_items[slotNo];
}

int Inventory::getQuickSlotItemId(int slotNo)
{
	if (slotNo < 0 || slotNo >= C_MAX_HOTBAR_ITEMS)
		return -1;
	
	int idx = m_hotbar[slotNo];
	ItemInstance* pInst = getItem(idx);
	if (!pInst)
		return -1;

	return pInst->m_itemID;
}

ItemInstance* Inventory::getQuickSlotItem(int slotNo)
{
	if (slotNo < 0 || slotNo >= C_MAX_HOTBAR_ITEMS)
		return nullptr;
	
	ItemInstance* pInst = getItem(m_hotbar[slotNo]);
	if (!pInst)
		return nullptr;

	if (pInst->m_itemID == 0)
		return nullptr;

	return pInst;
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

	if (m_bIsSurvival)
		return; // TODO

	for (int i = 0; i < getNumItems(); i++)
	{
		if (m_items[i].m_itemID == itemID)
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
		if (m_items[i].m_itemID != itemID)
			continue;

		selectItem(i, maxHotBarSlot);
		return;
	}

	LOG_W("selectItemById: %d doesn't exist", itemID);
}

int Inventory::getAttackDamage(Entity* pEnt)
{
	ItemInstance* pInst = getSelected();
	if (!pInst)
		return 1;

	return pInst->getAttackDamage(pEnt);
}