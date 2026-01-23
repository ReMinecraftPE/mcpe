#pragma once

#include "Slot.hpp"
#include "world/Container.hpp"

class ResultSlot : public Slot
{
public:
    ResultSlot(Player* player, Container* craftSlots, Container* resultSlots, int slotIndex, int x, int y);

    bool mayPlace(const ItemInstance& item) const override;
    bool canSync() override;
    void onTake(ItemInstance&) override;

private:
    Player* m_pPlayer;
    Container* m_pCraftSlots;
};
