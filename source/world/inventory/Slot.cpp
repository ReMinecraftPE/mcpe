#include "Slot.hpp"

Slot::Slot(Container* container, int slot, int x, int y) :
    m_pContainer(container),
    m_slot(slot),
    m_x(x),
    m_y(y)
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
