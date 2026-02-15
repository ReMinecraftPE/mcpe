#pragma once

#include "ContainerScreen.hpp"

class ClassicCraftingScreen_Console : public ContainerScreen
{
public:
    ClassicCraftingScreen_Console(Inventory* inventory, const TilePos& tilePos, Level* level);

    void init() override;
    void renderBackground() override;

protected:
    void _renderLabels() override;
    void _renderBg(float partialTick) override;
    SlotDisplay _createSlotDisplay(const Slot&);
};
