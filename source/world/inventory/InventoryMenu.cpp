#include "InventoryMenu.hpp"
#include "world/item/Inventory.hpp"
#include "ResultSlot.hpp"
#include "ArmorSlot.hpp"
//#include "world/item/crafting/Recipes.hpp"

InventoryMenu::InventoryMenu(Inventory* inventory, bool active) :
    m_pCraftSlots(nullptr),
    m_pResultSlots(nullptr),
    m_bActive(active) 
{
    m_pCraftSlots = new CraftingContainer(this, 2, 2);
    m_pResultSlots = new ResultContainer;

    addSlot(new ResultSlot(inventory->m_pPlayer, m_pCraftSlots, m_pResultSlots, 0, 144, 36));

    for (int y = 0; y < 2; ++y)
    {
        for (int x = 0; x < 2; ++x)
            addSlot(new Slot(m_pCraftSlots, x + y * 2, 88 + x * 18, 26 + y * 18));
    }

    for (int i = Item::SLOT_FEET; i <= Item::SLOT_HEAD; ++i)
    {
        addSlot(new ArmorSlot(inventory, Item::EquipmentSlot(Item::SLOT_HEAD - i), inventory->getContainerSize() - 1 - i, 8, 8 + i * 18));
    }

    for (int y = 0; y < 3; ++y)
    {
        for (int x = 0; x < 9; ++x)
            addSlot(new Slot(inventory, x + (y + 1) * 9, 8 + x * 18, 84 + y * 18));
    }

    for (int i = 0; i < 9; ++i)
    {
        addSlot(new Slot(inventory, i, 8 + i * 18, 142));
    }

    slotsChanged(m_pCraftSlots);
}

InventoryMenu::~InventoryMenu()
{
    delete m_pCraftSlots;
    delete m_pResultSlots;
}

void InventoryMenu::slotsChanged(Container* container) 
{
    //m_pResultSlots->setItem(0, Recipes::singleton().getItemFor(m_pCraftSlots));
}

void InventoryMenu::removed(Player* player) 
{
    ContainerMenu::removed(player);
    for (int i = 0; i < 4; ++i)
    {
        ItemStack& item = m_pCraftSlots->getItem(i);
        if (!item.isEmpty())
        {
            player->drop(item);
            m_pCraftSlots->setItem(i, ItemStack::EMPTY);
        }
    }
}

bool InventoryMenu::stillValid(Player* player) const 
{
    return true;
}

ItemStack InventoryMenu::quickMoveStack(int index)
{
    ItemStack item = ItemStack::EMPTY;
    Slot* slot = getSlot(index);
    if (slot && slot->hasItem())
    {
        ItemStack& slotItem = slot->getItem();
        item = slotItem;
        if (index == 0)
            moveItemStackTo(slotItem, 9, 45, true);
        else if (index >= 9 && index < 36)
            moveItemStackTo(slotItem, 36, 45, false);
        else if (index >= 36 && index < 45)
            moveItemStackTo(slotItem, 9, 36, false);
        else
            moveItemStackTo(slotItem, 9, 45, false);

        if (slotItem.m_count == 0)
            slot->set(ItemStack::EMPTY);
        else
            slot->setChanged();

        if (slotItem.m_count == item.m_count)
            return ItemStack::EMPTY;

        slot->onTake(slotItem);
    }

    return item;
}
