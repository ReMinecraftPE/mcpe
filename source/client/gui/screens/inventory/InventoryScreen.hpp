#pragma once

#include "ContainerScreen.hpp"

class InventoryScreen : public ContainerScreen
{

public:
    InventoryScreen(Player* player);

protected:
    virtual void _renderLabels() override;
    virtual void _renderBg(float partialTick) override;
};