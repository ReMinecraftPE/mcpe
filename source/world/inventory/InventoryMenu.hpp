#pragma once

#include "ContainerMenu.hpp"
#include "CraftingContainer.hpp"
#include "ResultContainer.hpp"

class InventoryMenu : public ContainerMenu
{
public:
    InventoryMenu(Inventory* inventory, bool active = true);
    ~InventoryMenu();

    void slotsChanged(Container* container) override;
    void removed(Player* player) override;
    bool stillValid(Player* player) const override;
    virtual ItemInstance quickMoveStack(int index) override;

public:
    CraftingContainer* m_pCraftSlots;
    ResultContainer* m_pResultSlots;
    // Unused member
    bool m_bActive;
};
