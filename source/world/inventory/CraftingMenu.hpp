#pragma once

#include "ContainerMenu.hpp"
#include "CraftingContainer.hpp"
#include "ResultContainer.hpp"

class CraftingMenu : public ContainerMenu
{
public:
    CraftingMenu(Inventory* inventory, const TilePos& tilePos, Level* level);
    ~CraftingMenu();

    void slotsChanged(Container* container) override;
    void removed(Player* player) override;
    bool stillValid(Player* player) const override;
    virtual ItemStack quickMoveStack(int index) override;

public:
    CraftingContainer* m_pCraftSlots;
    ResultContainer* m_pResultSlots;

private:
    const TilePos m_pos;
    Level* m_pLevel;
};
