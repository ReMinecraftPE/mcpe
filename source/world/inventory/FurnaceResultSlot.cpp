#include "FurnaceResultSlot.hpp"
#include "../item/Item.hpp"
#include "world/entity/Player.hpp"

FurnaceResultSlot::FurnaceResultSlot(Player* player, Container* container, int slotIndex)
    : Slot(container, slotIndex, OUTPUT), m_pPlayer(player)
{
}

bool FurnaceResultSlot::mayPlace(const ItemStack&) const
{
    return false;
}

void FurnaceResultSlot::onTake(ItemStack& inst)
{
    inst.onCraftedBy(m_pPlayer, m_pPlayer->m_pLevel);
	Slot::onTake(inst);
}
