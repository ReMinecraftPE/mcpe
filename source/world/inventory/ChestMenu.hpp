#pragma once

#include "ContainerMenu.hpp"
#include "world/Container.hpp"
#include "world/entity/Player.hpp"

class ChestMenu : public ContainerMenu
{
public:
    ChestMenu(Container* inventory, Container* container);

    bool stillValid(Player* player) const override;
    ItemStack quickMoveStack(int index) override;

private:
    Container* m_pContainer;
};
