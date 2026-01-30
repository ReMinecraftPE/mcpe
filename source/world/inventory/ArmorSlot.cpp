#include "ArmorSlot.hpp"
#include "world/tile/Tile.hpp"
#include "world/item/Item.hpp"

ArmorSlot::ArmorSlot(Container* container, Item::EquipmentSlot equipmentSlot, int slotIndex) :
    Slot(container, slotIndex, ARMOR),
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
