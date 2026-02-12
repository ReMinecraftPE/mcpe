#pragma once

#include "ContainerScreen.hpp"

class InventoryScreen : public ContainerScreen
{

public:
    InventoryScreen(Player* player);

    void init() override;
    void renderBackground() override;

protected:
    void _renderLabels() override;
    void _renderBg(float partialTick) override;
    SlotDisplay _createSlotDisplay(const Slot&);

public:
    static std::string CONSOLE_ARMOR_SLOTS[];
};