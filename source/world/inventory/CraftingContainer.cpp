#include "CraftingContainer.hpp"

CraftingContainer::CraftingContainer(ContainerMenu* menu, int width, int height) :
    m_items(width * height), m_pMenu(menu), m_width(width)
{
}

uint16_t CraftingContainer::getContainerSize() const
{
    return uint16_t(m_items.size());
}

ItemInstance& CraftingContainer::getItem(int index)
{
    return m_items[index];
}

ItemInstance& CraftingContainer::getItem(int x, int y)
{
    return getItem(x + y * m_width);
}

std::string CraftingContainer::getName() const
{
    return "Crafting";
}

ItemInstance CraftingContainer::removeItem(int index, int count)
{
    if (index < 0 || index >= getContainerSize()) return ItemInstance::EMPTY;

    ItemInstance& item = m_items[index];
    if (item)
    {
        ItemInstance removed;

        if (item.m_count <= count)
        {
            removed = item;
            m_items[index] = ItemInstance::EMPTY;
        } 
        else
        {
            removed = item.remove(count);
            if (item.m_count == 0)
                m_items[index] = ItemInstance::EMPTY;
        }

        m_pMenu->slotsChanged(this);
        return removed;
    }

    return ItemInstance::EMPTY;
}

void CraftingContainer::setItem(int index, const ItemInstance& item)
{
    if (index >= 0 && index < getContainerSize())
    {
        m_items[index] = item;
        m_pMenu->slotsChanged(this);
    }
}

void CraftingContainer::setChanged()
{
}

bool CraftingContainer::stillValid(Player* player)
{
    return false;
}