#pragma once

#include "ContainerScreen.hpp"
#include "world/tile/entity/FurnaceTileEntity.hpp"

class FurnaceScreen : public ContainerScreen
{
public:
    FurnaceScreen(Inventory* inventory, FurnaceTileEntity* container);
    void tick() override;

protected:
    void _renderLabels() override;
    void _renderBg(float a) override;
    SlotDisplay _createSlotDisplay(const Slot&) override;

private:
    Inventory* m_inventory;
    FurnaceTileEntity* m_furnace;
};
