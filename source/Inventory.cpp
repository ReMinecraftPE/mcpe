#include "Inventory.hpp"
#include "Item.hpp"

Inventory::Inventory(Player* pPlayer)
{
	m_pPlayer = pPlayer;
	m_SelectedHotbarSlot = 0;

	for (int i = 0; i < C_MAX_HOTBAR_ITEMS; i++)
		m_hotbar[i] = -1;
}

void Inventory::prepareCreativeInventory()
{
	m_bIsSurvival = false;

	m_items.clear();

	// add some items
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
	addCreativeItem(Tile::tnt->m_ID);
	addCreativeItem(Tile::gravel->m_ID);
	addCreativeItem(Tile::cloth->m_ID);
	addCreativeItem(Tile::mossStone->m_ID);
	addCreativeItem(Tile::bookshelf->m_ID);
	addCreativeItem(Tile::sponge->m_ID);
	addCreativeItem(Tile::sapling->m_ID);
	addCreativeItem(Tile::water->m_ID);
	addCreativeItem(Tile::lava->m_ID);
	addCreativeItem(Tile::fire->m_ID);
	addCreativeItem(Item::camera->m_itemID);
	addCreativeItem(Item::door_wood->m_itemID);
	addCreativeItem(Item::door_iron->m_itemID);

	for (int i = C_MAX_HOTBAR_ITEMS - 1; i >= 0; i--)
		selectItem(i);
}

void Inventory::prepareSurvivalInventory()
{
	m_bIsSurvival = true;
	m_items.clear();
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
	m_items.emplace_back(ItemInstance(itemID, 1, auxValue));
}

ItemInstance* Inventory::getItem(int slotNo)
{
	if (slotNo < 0 || slotNo >= int(m_items.size()))
		return nullptr;

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

int Inventory::getSelectedItemId()
{
	return getQuickSlotItemId(m_SelectedHotbarSlot);
}

void Inventory::selectItem(int slotNo)
{
	if (slotNo < 0 || slotNo >= getNumItems())
		return;

	// look for it in the hotbar
	for (int i = 0; i < C_MAX_HOTBAR_ITEMS; i++)
	{
		if (m_hotbar[i] == slotNo)
		{
			m_SelectedHotbarSlot = i;
			return;
		}
	}

	for (int i = C_MAX_HOTBAR_ITEMS - 2; i >= 0; i--)
		m_hotbar[i + 1] = m_hotbar[i];

	m_hotbar[0] = slotNo;
	m_SelectedHotbarSlot = 0;
}

void Inventory::selectSlot(int slotNo)
{
	if (slotNo < 0 || slotNo >= C_MAX_HOTBAR_ITEMS)
		return;

	m_SelectedHotbarSlot = slotNo;
}

void Inventory::setQuickSlotIndexByItemId(int slotNo, int itemID)
{
	if (slotNo < 0 || slotNo >= C_MAX_HOTBAR_ITEMS)
		return;

	// TODO: survival mode handling
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

void Inventory::selectItemById(int itemID)
{
	for (int i = 0; i < getNumItems(); i++)
	{
		if (m_items[i].m_itemID != itemID)
			continue;

		selectItem(i);
		return;
	}
}
