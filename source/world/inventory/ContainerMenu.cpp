#include "ContainerMenu.hpp"
#include "world/item/ItemStack.hpp"
#include "world/item/Inventory.hpp"
#include "Slot.hpp"
#include "Container.hpp"
#include "ContainerListener.hpp"

ContainerMenu::ContainerMenu(Container::Type containerType)
    : m_changeUid(0)
    , m_containerId(0)
    , m_containerType(containerType)
    , m_bBroadcastChanges(true)
{
}

ContainerMenu::~ContainerMenu()
{
    // Why were we doing this? The Player can listen to its own Inventory in certain cases
    /*for (std::vector<ContainerListener*>::iterator it = m_listeners.begin(); it != m_listeners.end(); ++it)
        delete (*it);*/

    _clearSlots();
}

void ContainerMenu::_clearSlots()
{
    for (std::vector<Slot*>::iterator it = m_slots.begin(); it != m_slots.end(); ++it)
    {
        Slot* slot = *it;

        // @HACK: I don't like this
        Container* pContainer = slot->m_pContainer;
        if (pContainer)
            pContainer->removeContentChangeListener(this);

        delete slot;
    }

    m_slots.clear();
}

void ContainerMenu::addSlot(Slot* slot)
{
    slot->m_index = m_slots.size();
    m_slots.push_back(slot);
    m_lastSlots.push_back(ItemStack::EMPTY);

    // @HACK: holy hack
    Container* pContainer = slot->m_pContainer;
    if (pContainer)
        pContainer->addContentChangeListener(this);
}

void ContainerMenu::addSlotListener(ContainerListener* listener)
{
    m_listeners.insert(listener);

    // Not done on PE
    /*std::vector<ItemStack> snapshot = cloneItems();
    listener->refreshContainer(this, snapshot);
    broadcastChanges();*/
}

void ContainerMenu::sendData(int id, int value)
{
    for (ContainerListeners::iterator it = m_listeners.begin(); it != m_listeners.end(); ++it)
        (*it)->setContainerData(this, id, value);
}

void ContainerMenu::broadcastChanges(SlotID slot)
{
    ItemStack& current = m_slots[slot]->getItem();
    if (m_lastSlots[slot] != current)
    {
        m_lastSlots[slot] = ItemStack(current);
        for (ContainerListeners::iterator it = m_listeners.begin(); it != m_listeners.end(); ++it)
            (*it)->slotChanged(this, slot, m_lastSlots[slot], isResultSlot());
    }
}

void ContainerMenu::broadcastChanges()
{
    for (size_t i = 0; i < m_slots.size(); ++i)
    {
        broadcastChanges(i);
    }
}

void ContainerMenu::removed(Player* player)
{
    Inventory* inv = player->m_pInventory;
    if (!inv->getCarried().isEmpty())
    {
        player->drop(inv->getCarried());
        inv->setCarried(ItemStack::EMPTY);
    }
}

void ContainerMenu::slotsChanged(Container*)
{
    broadcastChanges();
}

std::vector<ItemStack> ContainerMenu::cloneItems()
{
    std::vector<ItemStack> content;

    for (std::vector<Slot*>::iterator it = m_slots.begin(); it != m_slots.end(); ++it)
    {
        const ItemStack& item = (*it)->getItem();
        content.push_back(item);
    }

    return content;
}

Slot* ContainerMenu::getSlotFor(Container* container, int index)
{
    for (std::vector<Slot*>::iterator it = m_slots.begin(); it != m_slots.end(); ++it)
    {
        Slot* slot = *it;
        if (slot->isAt(container, index))
            return slot;
    }
    return nullptr;
}

Slot* ContainerMenu::getSlot(int index)
{
    return m_slots[index];
}

ItemStack ContainerMenu::quickMoveStack(int index)
{
    return index >= 0 && index < int(m_slots.size()) ? getSlot(index)->getItem() : ItemStack::EMPTY;
}

void ContainerMenu::moveItemStackTo(ItemStack& item, int slotFrom, int slotTo, bool take)
{
    int index = slotFrom;
    if (take)
        index = slotTo - 1;

    if (item.isStackable())
    {
        while (item.m_count > 0 && ((!take && index < slotTo) || (take && index >= slotFrom)))
        {
            Slot* slot = getSlot(index);
            ItemStack& slotItem = slot->getItem();
            if (slotItem && slotItem.getId() == item.getId() && (!item.isStackedByData() || item.getAuxValue() == slotItem.getAuxValue()))
            {
                int16_t sum = slotItem.m_count + item.m_count;
                if (sum <= item.getMaxStackSize())
                {
                    item.m_count = 0;
                    slotItem.m_count = sum;
                    slot->setChanged();
                }
                else if (slotItem.m_count < item.getMaxStackSize())
                {
                    item.m_count -= item.getMaxStackSize() - slotItem.m_count;
                    slotItem.m_count = item.getMaxStackSize();
                    slot->setChanged();
                }
            }

            if (take)
                --index;
            else
                ++index;
        }
    }

    if (item.m_count > 0)
    {
        if (take)
            index = slotTo - 1;
        else
            index = slotFrom;

        while ((!take && index < slotTo) || (take && index >= slotFrom))
        {
            Slot* slot = getSlot(index);
            ItemStack& slotItem = slot->getItem();
            if (!slotItem)
            {
                slot->set(item);
                slot->setChanged();
                item.m_count = 0;
                break;
            }

            if (take)
                --index;
            else
                ++index;
        }
    }
}

ItemStack ContainerMenu::clicked(int slotIndex, MouseButtonType mouseButton, bool quickMove, Player* player)
{
    ItemStack result = ItemStack::EMPTY;

    if (mouseButton != MOUSE_BUTTON_LEFT && mouseButton != MOUSE_BUTTON_RIGHT)
        return result;

    Inventory* inv = player->m_pInventory;

    if (slotIndex == -999)
    {
        if (!inv->getCarried().isEmpty())
        {
            switch (mouseButton)
            {
            case MOUSE_BUTTON_LEFT:
                player->drop(inv->getCarried());
                inv->setCarried(ItemStack::EMPTY);
                break;
            case MOUSE_BUTTON_RIGHT:
                player->drop(inv->getCarried().remove(1));
                if (!inv->getCarried().m_count)
                    inv->setCarried(ItemStack::EMPTY);
                break;
            default:
                break;
            }
        }
    }
    else
    {
        if (quickMove)
        {
            ItemStack quickMoved = quickMoveStack(slotIndex);
            if (!quickMoved.isEmpty())
            {
                result = quickMoved;
                Slot* slot = getSlot(slotIndex);
                if (slot && slot->hasItem() && slot->getItem().m_count < quickMoved.m_count)
                    clicked(slotIndex, mouseButton, quickMove, player);
            }

            return result;
        }

        Slot* slot = getSlot(slotIndex);
        if (!slot)
            return result;

        slot->setChanged();

        ItemStack& slotItem = slot->getItem();

        if (!slotItem.isEmpty())
            result = slotItem;

        ItemStack& carried = inv->getCarried();

        if (slotItem.isEmpty() && carried.isEmpty())
            return result;

        int16_t count;
        if (!slotItem.isEmpty() && carried.isEmpty())
        {
            count = (mouseButton == MOUSE_BUTTON_LEFT) ? slotItem.m_count : (slotItem.m_count + 1) / 2;
            inv->setCarried(slot->remove(count));
            if (!slotItem.m_count)
                slot->set(ItemStack::EMPTY);

            slot->onTake(inv->getCarried());
        }
        else if (slotItem.isEmpty() && !carried.isEmpty() && slot->mayPlace(carried))
        {
            count = (mouseButton == MOUSE_BUTTON_LEFT) ? carried.m_count : 1;
            if (count > slot->getMaxStackSize())
                count = slot->getMaxStackSize();

            slot->set(carried.remove(count));

            if (!carried.m_count)
                inv->setCarried(ItemStack::EMPTY);
        }
        else if (!slotItem.isEmpty() && !carried.isEmpty())
        {
            if (slot->mayPlace(carried))
            {
                if (slotItem.getId() != carried.getId() || (slotItem.isStackedByData() && slotItem.getAuxValue() != carried.getAuxValue()))
                {
                    if (carried.m_count <= slot->getMaxStackSize())
                    {
                        std::swap(carried, slotItem);
                    }
                }
                else if (slotItem.getId() == carried.getId())
                {
                    switch(mouseButton)
                    {
                    case MOUSE_BUTTON_LEFT:
                    {
                        count = carried.m_count;
                        int space = slot->getMaxStackSize() - slotItem.m_count;
                        if (count > space) count = space;

                        int maxStack = carried.getMaxStackSize() - slotItem.m_count;
                        if (count > maxStack) count = maxStack;

                        carried.remove(count);
                        if (!carried.m_count)
                            inv->setCarried(ItemStack::EMPTY);

                        slotItem.m_count += count;
                        break;
                    }
                    case MOUSE_BUTTON_RIGHT:
                    {
                        count = 1;
                        int space = slot->getMaxStackSize() - slotItem.m_count;
                        if (count > space) count = space;

                        int maxStack = carried.getMaxStackSize() - slotItem.m_count;
                        if (count > maxStack) count = maxStack;

                        carried.remove(count);
                        if (carried.m_count == 0)
                            inv->setCarried(ItemStack::EMPTY);

                        slotItem.m_count += count;
                        break;
                    }
                    default:
                        break;
                    }
                }
            }
            else
            {
                if (slotItem.getId() == carried.getId() && carried.getMaxStackSize() > 1 &&
                    (!slotItem.isStackedByData() || slotItem.getAuxValue() == carried.getAuxValue()))
                {
                    int16_t slotCount = slotItem.m_count;
                    if (slotCount > 0 && (slotCount + carried.m_count) <= carried.getMaxStackSize())
                    {
                        carried.m_count += slotCount;
                        slotItem.remove(slotCount);
                        if (slotItem.m_count == 0)
                            slot->set(ItemStack::EMPTY);

                        slot->onTake(carried);
                    }
                }
            }
        }
    }

    return result;
}

void ContainerMenu::setItem(int index, ItemStack item)
{
    m_slots[index]->set(item);
}

void ContainerMenu::setAll(const std::vector<ItemStack>& items)
{
    size_t n = std::min(items.size(), m_slots.size());
    for (size_t i = 0; i < n; ++i)
    {
        m_slots[i]->set(items[i]);
    }
}

void ContainerMenu::setData(int id, int value)
{
}

uint16_t ContainerMenu::backup(Inventory*)
{
    return ++m_changeUid;
}

void ContainerMenu::deleteBackup(uint16_t)
{
}

void ContainerMenu::rollbackToBackup(uint16_t)
{
}

bool ContainerMenu::isSynched(Player* player) const
{
    return m_unsynchedPlayers.find(player) == m_unsynchedPlayers.end();
}

void ContainerMenu::setSynched(Player* player, bool isSynched)
{
    if (isSynched)
        m_unsynchedPlayers.erase(player);
    else
        m_unsynchedPlayers.insert(player);
}

void ContainerMenu::containerContentChanged(SlotID slot)
{
    if (m_bBroadcastChanges)
        broadcastChanges(slot);
}
