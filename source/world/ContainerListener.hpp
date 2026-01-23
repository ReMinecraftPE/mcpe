#pragma once

#include <vector>

class ContainerMenu;
class ItemInstance;

class ContainerListener
{
public:
    virtual void refreshContainer(ContainerMenu* menu, const std::vector<ItemInstance>& items) = 0;
    virtual void refreshContainerItems(ContainerMenu* menu);
    virtual void slotChanged(ContainerMenu* menu, int index, ItemInstance& item) = 0;
    virtual void setContainerData(ContainerMenu* menu, int id, int value) = 0;
};