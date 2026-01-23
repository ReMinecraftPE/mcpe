#include "ResultContainer.hpp"

ResultContainer::ResultContainer()
{
}

uint16_t ResultContainer::getContainerSize() const
{
    return 1;
}

ItemInstance& ResultContainer::getItem(int index)
{
    return m_item;
}

std::string ResultContainer::getName() const
{
    return "";
}

ItemInstance ResultContainer::removeItem(int index, int)
{
    if (index == 0)
    {
        ItemInstance result = m_item;
        m_item = ItemInstance::EMPTY;
        return result;
    }
    return ItemInstance::EMPTY;
}

void ResultContainer::setItem(int index, const ItemInstance& item)
{
    if (index == 0)
        m_item = item;
}

void ResultContainer::setChanged()
{
}

bool ResultContainer::stillValid(Player*) {
    return true;
}
