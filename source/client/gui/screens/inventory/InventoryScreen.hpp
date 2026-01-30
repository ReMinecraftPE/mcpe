#pragma once

#include "ContainerScreen.hpp"

class InventoryScreen : public ContainerScreen
{

public:
    InventoryScreen(Player* player);

    virtual void init() override;
    virtual void renderBackground() override;

protected:
    virtual void _renderLabels() override;
    virtual void _renderBg(float partialTick) override;
};