#pragma once

#include <vector>

class ContainerMenu;
class ItemStack;
class Slot;

class ContainerListener
{
public:
    virtual ~ContainerListener();

    virtual void refreshContainer(ContainerMenu* menu, const std::vector<ItemStack>& items) {}
    virtual void refreshContainerItems(ContainerMenu* menu);
    virtual void slotChanged(ContainerMenu* menu, int index, Slot* slot, ItemStack& item, bool isResultSlot) {}
    virtual void setContainerData(ContainerMenu* menu, int id, int value) {}
};