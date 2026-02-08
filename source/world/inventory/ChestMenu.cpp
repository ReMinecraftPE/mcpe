#include "ChestMenu.hpp"
#include "Slot.hpp"

ChestMenu::ChestMenu(Container* inventory, Container* container)
    : ContainerMenu(Container::CONTAINER)
    , m_pContainer(container)
{
    int rows = m_pContainer->getContainerSize() / 9;

    for (int row = 0; row < rows; ++row)
    {
        for (int col = 0; col < 9; ++col)
            addSlot(new Slot(m_pContainer, col + row * 9));
    }

    for (int row = 0; row < 3; ++row)
    {
        for (int col = 0; col < 9; ++col)
            addSlot(new Slot(inventory, col + row * 9 + 9));
    }

    for (int col = 0; col < 9; ++col)
    {
        addSlot(new Slot(inventory, col, Slot::HOTBAR));
    }
}

bool ChestMenu::stillValid(Player* player) const
{
    return m_pContainer->stillValid(player);
}

ItemStack ChestMenu::quickMoveStack(int index)
{
    ItemStack item = ItemStack::EMPTY;
    Slot* slot = getSlot(index);
    if (slot && slot->hasItem())
    {
        ItemStack& slotItem = slot->getItem();
        item = slotItem;
        int rows = m_pContainer->getContainerSize() / 9;
        if (index < rows * 9)
            moveItemStackTo(slotItem, rows * 9, m_slots.size(), true);
        else
            moveItemStackTo(slotItem, 0, rows * 9, false);

        if (slotItem.m_count == 0)
            slot->set(ItemStack::EMPTY);
        else
            slot->setChanged();

        if (slotItem.m_count == item.m_count)
            return ItemStack::EMPTY;
    }

    return item;
}
