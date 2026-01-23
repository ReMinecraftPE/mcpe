#pragma once

#include "world/Container.hpp"

class ItemInstance;

class Slot
{
public:
    Slot(Container* container, int slot, int x, int y);
    virtual ~Slot();

    virtual bool canSync();

    virtual void onTake(ItemInstance&)
    {
        setChanged();
    }

    virtual bool mayPlace(const ItemInstance& item)
    {
        return true;
    }

    virtual ItemInstance& getItem()
    {
        return m_pContainer->getItem(m_slot);
    }

    virtual bool hasItem()
    {
        return !getItem().isEmpty();
    }

    virtual void set(const ItemInstance& item)
    {
        m_pContainer->setItem(m_slot, item);
        setChanged();
    }

    virtual void setChanged()
    {
        m_pContainer->setChanged();
    }

    virtual int getMaxStackSize()
    {
        return m_pContainer->getMaxStackSize();
    }

    virtual int getNoItemIcon()
    {
        return -1;
    }

    virtual ItemInstance remove(int count)
    {
        return m_pContainer->removeItem(m_slot, count);
    }

    virtual bool isAt(Container* cont, int s)
    {
        return cont == m_pContainer && s == m_slot;
    }


protected:
    int m_slot;

public:
    int m_index;
    int m_x;
    int m_y;
    Container* m_pContainer;
};