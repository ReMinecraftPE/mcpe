#include "CraftingMenu.hpp"
#include "world/item/Inventory.hpp"
#include "ResultSlot.hpp"
#include "world/item/crafting/Recipes.hpp"
#include "world/level/Level.hpp"

CraftingMenu::CraftingMenu(Inventory* inventory, const TilePos& tilePos, Level* level) :
    m_pos(tilePos),
    m_pLevel(level)
{
    m_pCraftSlots = new CraftingContainer(this, 3, 3);
    m_pResultSlots = new ResultContainer();

    addSlot(new ResultSlot(inventory->m_pPlayer, m_pCraftSlots, m_pResultSlots, 0));

    for (int y = 0; y < 3; ++y)
    {
        for (int x = 0; x < 3; ++x)
            addSlot(new Slot(m_pCraftSlots, x + y * 3, Slot::INPUT));
    }

    for (int y = 0; y < 3; ++y)
    {
        for (int x = 0; x < 9; ++x)
            addSlot(new Slot(inventory, x + (y + 1) * 9, Slot::INVENTORY));
    }

    for (int i = 0; i < 9; ++i)
    {
        addSlot(new Slot(inventory, i, Slot::HOTBAR));
    }

    slotsChanged(m_pCraftSlots);
}

CraftingMenu::~CraftingMenu()
{
    delete m_pCraftSlots;
    delete m_pResultSlots;
}

void CraftingMenu::slotsChanged(Container* container) 
{
    m_pResultSlots->setItem(0, Recipes::singleton().getItemFor(m_pCraftSlots));
}

void CraftingMenu::removed(Player* player) 
{
    ContainerMenu::removed(player);
    for (int i = 0; i < 9; ++i)
    {
        ItemStack& item = m_pCraftSlots->getItem(i);
        if (!item.isEmpty())
        {
            player->drop(item);
            m_pCraftSlots->setItem(i, ItemStack::EMPTY);
        }
    }
}

bool CraftingMenu::stillValid(Player* player) const 
{
    if (m_pLevel->getTile(m_pos) != Tile::craftingTable->m_ID)
        return false;
    else
        return !(player->distanceToSqr(Vec3(m_pos.x + 0.5f, m_pos.y + 0.5f, m_pos.z + 0.5)) > 64.0f);
}

ItemStack CraftingMenu::quickMoveStack(int index)
{
    ItemStack item = ItemStack::EMPTY;
    Slot* slot = getSlot(index);
    if (slot && slot->hasItem())
    {
        ItemStack& slotItem = slot->getItem();
        item = slotItem;
        if (index == 0)
            moveItemStackTo(slotItem, 10, 46, true);
        else if (index >= 10 && index < 37)
            moveItemStackTo(slotItem, 37, 46, false);
        else if (index >= 37 && index < 46)
            moveItemStackTo(slotItem, 10, 37, false);
        else
            moveItemStackTo(slotItem, 10, 46, false);

        if (slotItem.m_count == 0)
            slot->set(ItemStack::EMPTY);
        else
            slot->setChanged();

        if (slotItem.m_count == item.m_count)
            return ItemStack::EMPTY;

        slot->onTake(item);
    }

    return item;
}
