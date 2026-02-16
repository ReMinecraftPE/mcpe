#pragma once

#include "ContainerMenu.hpp"
#include "world/entity/Player.hpp"
#include <world/tile/entity/FurnaceTileEntity.hpp>

class FurnaceMenu : public ContainerMenu
{
public:
    FurnaceMenu(Inventory* inventory, FurnaceTileEntity* container);

public:
    bool stillValid(Player* player) const override;
    void addSlotListener(ContainerListener* listener) override;
    void broadcastChanges() override;
    void setData(int, int) override;
    ItemStack quickMoveStack(int index) override;

private:
    FurnaceTileEntity* m_furnace;
    int m_lastCookTime;
    int m_lastBurnTime;
    int m_lastLitDuration;
};
