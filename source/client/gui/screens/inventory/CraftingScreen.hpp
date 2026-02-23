#pragma once

#include "ContainerScreen.hpp"

class CraftingScreen : public ContainerScreen
{

public:
    CraftingScreen(Inventory* inventory, const TilePos& tilePos, Level* level);

protected:
    void _renderLabels() override;
    void _renderBg(float partialTick) override;
    SlotDisplay _createSlotDisplay(const Slot&);
};
