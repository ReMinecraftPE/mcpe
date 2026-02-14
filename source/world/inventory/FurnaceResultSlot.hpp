#pragma once

#include "Slot.hpp"
#include "source/world/Container.hpp"

class FurnaceResultSlot : public Slot
{
public:
    FurnaceResultSlot(Player* player, Container* container, int slotIdx, int x, int y);

    bool mayPlace(const ItemStack&) const override;
    void onTake(ItemStack&) override;

private:
    Player* m_pPlayer;
};