#include "FurnaceMenu.hpp"
#include "Slot.hpp"
#include "FurnaceResultSlot.hpp"
#include "world/ContainerListener.hpp"

FurnaceMenu::FurnaceMenu(Inventory* inventory, FurnaceTileEntity* furnace)
    : ContainerMenu(Container::FURNACE), m_furnace(furnace), m_lastCookTime(0), m_lastBurnTime(0), m_lastLitDuration(0)
{
    addSlot(new Slot(m_furnace, 0, Slot::INPUT));
    addSlot(new Slot(m_furnace, 1, Slot::INPUT));
    addSlot(new FurnaceResultSlot(inventory->m_pPlayer, m_furnace, 2));

    for (int y = 0; y < 3; ++y)
    {
        for (int x = 0; x < 9; ++x)
            addSlot(new Slot(inventory, x + (y + 1) * 9, Slot::INVENTORY));
    }

    for (int i = 0; i < 9; ++i)
    {
        addSlot(new Slot(inventory, i, Slot::HOTBAR));
    }
}

bool FurnaceMenu::stillValid(Player* player) const
{
    return m_furnace->stillValid(player);
}

void FurnaceMenu::addSlotListener(ContainerListener* listener)
{
    ContainerMenu::addSlotListener(listener);
    listener->setContainerData(this, 0, m_furnace->m_tickCount);
    listener->setContainerData(this, 1, m_furnace->m_litTime);
    listener->setContainerData(this, 2, m_furnace->m_litDuration);
}

void FurnaceMenu::broadcastChanges()
{
    ContainerMenu::broadcastChanges();

    for (std::vector<ContainerListener*>::iterator it = m_listeners.begin(); it != m_listeners.end(); ++it)
    {
        ContainerListener* listener = *it;
        
        if (m_lastCookTime != m_furnace->m_tickCount)
            listener->setContainerData(this, 0, m_furnace->m_tickCount);

        if (m_lastBurnTime != m_furnace->m_litTime)
            listener->setContainerData(this, 1, m_furnace->m_litTime);

        if (m_lastLitDuration != m_furnace->m_litDuration)
            listener->setContainerData(this, 2, m_furnace->m_litDuration);
    }

    m_lastCookTime = m_furnace->m_tickCount;
    m_lastBurnTime = m_furnace->m_litTime;
    m_lastLitDuration = m_furnace->m_litDuration;
}

void FurnaceMenu::setData(int index, int value)
{
    if (index == 0)
        m_furnace->m_tickCount = value;
    else if (index == 1)
        m_furnace->m_litTime = value;
    else if (index == 2)
        m_furnace->m_litDuration = value;
}

ItemStack FurnaceMenu::quickMoveStack(int index)
{
    ItemStack item;
    Slot* slot = getSlot(index);
    if (slot && slot->hasItem())
    {
        ItemStack& slotItem = slot->getItem();
        item = slotItem.copy();
        if (index == 2)
            moveItemStackTo(slotItem, 3, 39, true);
        else if (index >= 3 && index < 30)
            moveItemStackTo(slotItem, 30, 39, false);
        else if (index >= 30 && index < 39)
            moveItemStackTo(slotItem, 3, 30, false);
        else
            moveItemStackTo(slotItem, 3, 39, false);

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
