#pragma once

#include <set>
#include <string>
#include "Container.hpp"
#include "ContainerContentChangeListener.hpp"

class CompoundContainer : public Container
{
private:
    class ChildListener;

    std::string m_name;
    Container* m_pLeftContainer;
    Container* m_pRightContainer;
    ContentChangeListeners m_contentChangeListeners;
    ChildListener* m_pLeftListener;
    ChildListener* m_pRightListener;

public:
    CompoundContainer(const std::string& name, Container* c1, Container* c2);
    ~CompoundContainer() override;

    uint16_t getContainerSize() const override;

    std::string getName() const override;

    ItemStack& getItem(int index) override;

    ItemStack removeItem(int index, int count) override;

    void setItem(int index, const ItemStack& item) override;

    int getMaxStackSize() override;

    void setContainerChanged(SlotID slot) override;

    bool stillValid(Player* player) const override;

    void addContentChangeListener(ContainerContentChangeListener* listener) override;
    void removeContentChangeListener(ContainerContentChangeListener* listener) override;
};
