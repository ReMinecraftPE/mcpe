#pragma once

#include "ContainerScreen.hpp"

class ChestScreen : public ContainerScreen
{
public:
    ChestScreen(Container* inventory, Container* container);

protected:
    void _renderLabels() override;
    void _renderBg(float partialTicks) override;

private:
    Container* m_pInventory;
    Container* m_pContainer;
    int m_containerRows;
};
