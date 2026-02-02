#pragma once

#include "ContainerScreen.hpp"

class CraftingScreen : public ContainerScreen
{

public:
    CraftingScreen(Inventory* inventory, const TilePos& tilePos, Level* level);

protected:
    virtual void _renderLabels() override;
    virtual void _renderBg(float partialTick) override;
};