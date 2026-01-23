#pragma once

#include <vector>
#include "world/Container.hpp"
#include "world/item/ItemInstance.hpp"
#include "ContainerMenu.hpp"

class ContainerMenu;
class Player;
class ItemInstance;

class CraftingContainer : public Container
{

public:
    CraftingContainer(ContainerMenu* menu, int width, int height);

    uint16_t getContainerSize() const override;
    ItemInstance& getItem(int index) override;
    ItemInstance& getItem(int x, int y);

    std::string getName() const override;

    ItemInstance removeItem(int index, int amount) override;
    void setItem(int index, const ItemInstance& item) override;

    void setChanged() override;
    bool stillValid(Player* player) override;

private:
    std::vector<ItemInstance> m_items;
    ContainerMenu* m_pMenu;
    int m_width;
};