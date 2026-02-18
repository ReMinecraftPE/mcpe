#pragma once

#include "world/item/ItemStack.hpp"
#include "Container.hpp"

class Player;

class ResultContainer : public Container
{
public:
    ResultContainer();
    virtual ~ResultContainer();

    uint16_t getContainerSize() const override;
    ItemStack& getItem(int index) override;

    std::string getName() const override;

    ItemStack removeItem(int index, int amount) override;
    void setItem(int index, const ItemStack& item) override;

    void setContainerChanged(SlotID slot) override;
    bool stillValid(Player* player) const override;

private:
    ItemStack m_item;
};
