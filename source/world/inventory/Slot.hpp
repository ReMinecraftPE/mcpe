#pragma once

#include "world/Container.hpp"

class ItemStack;

class Slot
{
public:
    Slot(Container* container, int slot, int x, int y);
    virtual ~Slot();

    virtual bool canSync() const;

    virtual void onTake(ItemStack&) { setChanged(); }

    virtual bool mayPlace(const ItemStack& item) const { return true; }

    virtual ItemStack& getItem() { return m_pContainer->getItem(m_slot); }

    virtual bool hasItem() { return !getItem().isEmpty(); }

    virtual void set(const ItemStack& item);

    virtual void setChanged() { m_pContainer->setChanged(); }

    virtual int getMaxStackSize() const { return m_pContainer->getMaxStackSize(); }

    virtual int getNoItemIcon() const { return -1; }

    virtual ItemStack remove(int count) { return m_pContainer->removeItem(m_slot, count); }

    virtual bool isAt(Container* cont, int s) { return cont == m_pContainer && s == m_slot; }

public:
    Container* m_pContainer;
    int m_slot;
    int m_index;
    int m_x;
    int m_y;
};