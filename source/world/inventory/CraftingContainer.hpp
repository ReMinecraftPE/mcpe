#pragma once

#include <vector>
#include "world/item/ItemStack.hpp"
#include "Container.hpp"
#include "ContainerMenu.hpp"

class ContainerMenu;
class Player;
class ItemStack;

class CraftingContainer : public Container
{

public:
    CraftingContainer(ContainerMenu* menu, int width, int height);
    virtual ~CraftingContainer();

    uint16_t getContainerSize() const override;
    ItemStack& getItem(int index) override;
    const ItemStack& getItem(int x, int y);

    std::string getName() const override;

    ItemStack removeItem(int index, int amount) override;
    void setItem(int index, const ItemStack& item) override;

    void setContainerChanged(SlotID slot) override;
    bool stillValid(Player* player) const override;

private:
    std::vector<ItemStack> m_items;
    ContainerMenu* m_pMenu;
    int m_width;
};