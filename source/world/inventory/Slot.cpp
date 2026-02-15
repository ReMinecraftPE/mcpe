#include "Slot.hpp"

Slot::Slot(Container* container, int slot, Group group) :
    m_pContainer(container),
    m_slot(slot),
    m_group(group)
{
}

Slot::~Slot()
{
}

bool Slot::canSync() const
{
    return true;
}

void Slot::set(const ItemStack& item)
{
    m_pContainer->setItem(m_slot, item);
    setChanged();
}
