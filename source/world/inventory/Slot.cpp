#include "Slot.hpp"

Slot::Slot(Container* container, int slot, int x, int y) :
    m_pContainer(container), m_slot(slot), m_x(x), m_y(y) {
}

bool Slot::canSync()
{
    return true;
}
