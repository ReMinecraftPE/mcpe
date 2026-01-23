#include "ArmorSlot.hpp"
#include "world/tile/Tile.hpp"
#include "world/item/Item.hpp"

ArmorSlot::ArmorSlot(Container* container, int armorIndex, int slotIndex, int x, int y) : Slot(container, slotIndex, x, y), m_armorIndex(armorIndex)
{
}

bool ArmorSlot::mayPlace(const ItemInstance& item) const
{
     if (item.getItem()->getEquipmentSlot() == 3 - m_armorIndex)
         return true;
     else if (item.getItem()->m_itemID == Tile::pumpkin->m_ID)
         return m_armorIndex == 0;
     return false;
}

int ArmorSlot::getMaxStackSize()
{
    return 1;
}

int ArmorSlot::getNoItemIcon()
{
    return 16 * (m_armorIndex + 1) - 1;
}
