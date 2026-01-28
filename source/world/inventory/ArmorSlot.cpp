#include "ArmorSlot.hpp"
#include "world/tile/Tile.hpp"
#include "world/item/Item.hpp"

ArmorSlot::ArmorSlot(Container* container, Item::EquipmentSlot equipmentSlot, int slotIndex, int x, int y) :
    Slot(container, slotIndex, x, y),
    m_equipmentSlot(equipmentSlot)
{
}

bool ArmorSlot::mayPlace(const ItemStack& item) const
{
    if (item.getItem()->getEquipmentSlot() == m_equipmentSlot)
        return true;
    else if (item.getItem()->m_itemID == Tile::pumpkin->m_ID)
        return m_equipmentSlot == Item::SLOT_HEAD;
    return false;
}

int ArmorSlot::getMaxStackSize() const
{
    return 1;
}

int ArmorSlot::getNoItemIcon() const
{
    return 16 * ((Item::SLOT_HEAD - m_equipmentSlot) + 1) - 1;
}
