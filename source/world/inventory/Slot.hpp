#pragma once

#include "world/Container.hpp"

class ItemStack;

class Slot
{
public:
    enum Group
    {
        CONTAINER,
        INVENTORY,
        HOTBAR,
        INPUT,
        OUTPUT,
        ARMOR
    };

    Slot(Container* container, int slot, Group group = CONTAINER);
    virtual ~Slot();

    virtual bool canSync() const;

    virtual void onTake(ItemStack&) { setChanged(); }

    virtual bool mayPlace(const ItemStack& item) const { return true; }

    virtual ItemStack& getItem() { return m_pContainer->getItem(m_slot); }

    virtual bool hasItem() { return !getItem().isEmpty(); }

    virtual void set(const ItemStack& item);

    virtual void setChanged() { m_pContainer->setChanged(); }

    virtual int getMaxStackSize() const { return m_pContainer->getMaxStackSize(); }

    virtual ItemStack remove(int count) { return m_pContainer->removeItem(m_slot, count); }

    virtual bool isAt(Container* cont, int s) { return cont == m_pContainer && s == m_slot; }

public:
    Container* m_pContainer;
    int m_slot;
    int m_index;
    Group m_group;
};