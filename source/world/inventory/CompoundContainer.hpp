#pragma once

#include <string>
#include "Container.hpp"

class CompoundContainer : public Container
{
private:
    std::string m_name;
    Container* m_pLeftContainer;
    Container* m_pRightContainer;

public:
    CompoundContainer(const std::string& name, Container* c1, Container* c2);

    uint16_t getContainerSize() const override;

    std::string getName() const override;

    ItemStack& getItem(int index) override;

    ItemStack removeItem(int index, int count) override;

    void setItem(int index, const ItemStack& item) override;

    int getMaxStackSize() override;

    void setContainerChanged(SlotID slot) override;

    bool stillValid(Player* player) const override;
};
