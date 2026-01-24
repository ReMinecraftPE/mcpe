#include "CompoundContainer.hpp"

CompoundContainer::CompoundContainer(const std::string& name, Container* c1, Container* c2) :
    m_name(name), m_pLeftContainer(c1), m_pRightContainer(c2)
{
}

uint16_t CompoundContainer::getContainerSize() const
{
    return uint16_t(m_pLeftContainer->getContainerSize() + m_pRightContainer->getContainerSize());
}

std::string CompoundContainer::getName() const
{
    return m_name;
}

ItemStack& CompoundContainer::getItem(int index)
{
    if (index >= m_pLeftContainer->getContainerSize())
        return m_pRightContainer->getItem(index - m_pLeftContainer->getContainerSize());
    else
        return m_pLeftContainer->getItem(index);
}

ItemStack CompoundContainer::removeItem(int index, int count)
{
    if (index >= m_pLeftContainer->getContainerSize())
        return m_pRightContainer->removeItem(index - m_pLeftContainer->getContainerSize(), count);
    else
        return m_pLeftContainer->removeItem(index, count);
}

void CompoundContainer::setItem(int index, const ItemStack& item)
{
    if (index >= m_pLeftContainer->getContainerSize())
        m_pRightContainer->setItem(index - m_pLeftContainer->getContainerSize(), item);
    else
        m_pLeftContainer->setItem(index, item);
}

int CompoundContainer::getMaxStackSize()
{
    return m_pLeftContainer->getMaxStackSize();
}

void CompoundContainer::setChanged()
{
    m_pLeftContainer->setChanged();
    m_pRightContainer->setChanged();
}

bool CompoundContainer::stillValid(Player* player) const
{
    return m_pLeftContainer->stillValid(player) && m_pRightContainer->stillValid(player);
}

