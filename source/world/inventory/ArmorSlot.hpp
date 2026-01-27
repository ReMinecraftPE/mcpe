#pragma once

#include "Slot.hpp"
#include "world/Container.hpp"

class ArmorSlot : public Slot
{
public:
    ArmorSlot(Container* container, Item::EquipmentSlot equipmentSlot, int slotIndex, int x, int y);

    bool mayPlace(const ItemStack& item) const override;
    int getMaxStackSize() const override;
    int getNoItemIcon() const override;

public:
    Item::EquipmentSlot m_equipmentSlot;
};
