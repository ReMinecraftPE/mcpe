#pragma once

#include "Slot.hpp"
#include "world/Container.hpp"

class ArmorSlot : public Slot
{

public:
    int m_armorIndex;
    ArmorSlot(Container* container, int armorIndex, int slotIndex, int x, int y);

    bool mayPlace(const ItemInstance& item) const override;
    int getMaxStackSize() override;
    int getNoItemIcon() override;
};
