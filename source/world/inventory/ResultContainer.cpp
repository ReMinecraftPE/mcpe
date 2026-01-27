#include "ResultContainer.hpp"

ResultContainer::ResultContainer()
{
}

ResultContainer::~ResultContainer()
{
}

uint16_t ResultContainer::getContainerSize() const
{
    return 1;
}

ItemStack& ResultContainer::getItem(int index)
{
    return m_item;
}

std::string ResultContainer::getName() const
{
    return "";
}

ItemStack ResultContainer::removeItem(int index, int)
{
    if (index == 0)
    {
        ItemStack result = m_item;
        m_item = ItemStack::EMPTY;
        return result;
    }
    return ItemStack::EMPTY;
}

void ResultContainer::setItem(int index, const ItemStack& item)
{
    if (index == 0)
        m_item = item;
}

void ResultContainer::setChanged()
{
}

bool ResultContainer::stillValid(Player*) const
{
    return true;
}
