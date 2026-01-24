#pragma once

#include <vector>

class ContainerMenu;
class ItemStack;

class ContainerListener
{
public:
    virtual ~ContainerListener();

    virtual void refreshContainer(ContainerMenu* menu, const std::vector<ItemStack>& items) = 0;
    virtual void refreshContainerItems(ContainerMenu* menu);
    virtual void slotChanged(ContainerMenu* menu, int index, ItemStack& item) = 0;
    virtual void setContainerData(ContainerMenu* menu, int id, int value) = 0;
};