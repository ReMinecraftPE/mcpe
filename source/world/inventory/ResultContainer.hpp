#pragma once

#include "world/Container.hpp"
#include "world/item/ItemInstance.hpp"

class Player;

class ResultContainer : public Container
{
public:
    ResultContainer();

    uint16_t getContainerSize() const override;
    ItemInstance& getItem(int index) override;

    std::string getName() const override;

    ItemInstance removeItem(int index, int amount) override;
    void setItem(int index, const ItemInstance& item) override;

    void setChanged() override;
    bool stillValid(Player* player) override;

private:
    ItemInstance m_item;
};
