#pragma once

#include "Slot.hpp"
#include "Container.hpp"

class ResultSlot : public Slot
{
public:
    ResultSlot(Player* player, Container* craftSlots, Container* resultSlots, int slotIndex);

    bool mayPlace(const ItemStack& item) const override;
    bool canSync() const override;
    void onTake(ItemStack&) override;

private:
    Player* m_pPlayer;
    Container* m_pCraftSlots;
};
