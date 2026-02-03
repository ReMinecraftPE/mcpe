#include "CraftingContainer.hpp"

CraftingContainer::CraftingContainer(ContainerMenu* menu, int width, int height) :
    m_items(width * height),
    m_pMenu(menu),
    m_width(width)
{
}

CraftingContainer::~CraftingContainer()
{
}

uint16_t CraftingContainer::getContainerSize() const
{
    return uint16_t(m_items.size());
}

ItemStack& CraftingContainer::getItem(int index)
{
    return m_items[index];
}

const ItemStack& CraftingContainer::getItem(int x, int y)
{
    if (x >= 0 && x < m_width)
    {
        ItemStack* stack = tryGetItem(x + y * m_width);
        return stack ? *stack : ItemStack::EMPTY;
    }
    return ItemStack::EMPTY;
}

std::string CraftingContainer::getName() const
{
    return "Crafting";
}

ItemStack CraftingContainer::removeItem(int index, int count)
{
    if (index < 0 || index >= getContainerSize()) return ItemStack::EMPTY;

    ItemStack& item = m_items[index];
    if (item)
    {
        ItemStack removed;

        if (item.m_count <= count)
        {
            removed = item;
            m_items[index] = ItemStack::EMPTY;
        } 
        else
        {
            removed = item.remove(count);
            if (item.m_count == 0)
                m_items[index] = ItemStack::EMPTY;
        }

        m_pMenu->slotsChanged(this);
        return removed;
    }

    return ItemStack::EMPTY;
}

void CraftingContainer::setItem(int index, const ItemStack& item)
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

bool CraftingContainer::stillValid(Player* player) const
{
    return true;
}