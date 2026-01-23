#include "ContainerMenu.hpp"
#include "Slot.hpp"
#include "world/item/ItemInstance.hpp"
#include "world/item/Inventory.hpp"
#include "world/Container.hpp"
#include "world/ContainerListener.hpp"

void ContainerMenu::addSlot(Slot* slot)
{
    slot->m_index = m_slots.size();
    m_slots.push_back(slot);
    m_lastSlots.push_back(ItemInstance::EMPTY);
}

void ContainerMenu::addSlotListener(ContainerListener* listener)
{
    m_listeners.push_back(listener);

    std::vector<ItemInstance> snapshot;
    for (std::vector<Slot*>::iterator it = m_slots.begin(); it != m_slots.end(); ++it)
        snapshot.push_back((*it)->getItem());

    listener->refreshContainer(this, snapshot);
    broadcastChanges();
}

ContainerMenu::ContainerMenu() :
    m_changeUid(0),
    m_containerId(0)
{
}

ContainerMenu::~ContainerMenu()
{
    for (std::vector<ContainerListener*>::iterator it = m_listeners.begin(); it != m_listeners.end(); ++it)
        delete (*it);

    for (std::vector<Slot*>::iterator it = m_slots.begin(); it != m_slots.end(); ++it)
        delete (*it);
}

void ContainerMenu::sendData(int id, int value)
{
    for (std::vector<ContainerListener*>::iterator it = m_listeners.begin(); it != m_listeners.end(); ++it)
        (*it)->setContainerData(this, id, value);
}

void ContainerMenu::broadcastChanges()
{
    for (size_t i = 0; i < m_slots.size(); ++i)
    {
        ItemInstance& current = m_slots[i]->getItem();
        if (m_lastSlots[i] != current)
        {
            m_lastSlots[i] = ItemInstance(current);
            for (std::vector<ContainerListener*>::iterator it = m_listeners.begin(); it != m_listeners.end(); ++it)
                (*it)->slotChanged(this, i, m_lastSlots[i]);
        }
    }
}

void ContainerMenu::removed(Player* player)
{
    Inventory* inv = player->m_pInventory;
    if (!inv->getCarried().isEmpty())
    {
        player->drop(inv->getCarried());
        inv->setCarried(ItemInstance::EMPTY);
    }
}

void ContainerMenu::slotsChanged(Container*)
{
    broadcastChanges();
}

std::vector<ItemInstance> ContainerMenu::getItems()
{
    std::vector<ItemInstance> content;

    for (std::vector<Slot*>::iterator it = m_slots.begin(); it != m_slots.end(); ++it)
        content.push_back((*it)->getItem());

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

ItemInstance ContainerMenu::quickMoveStack(int index)
{
    return index >= 0 && index < int(m_slots.size()) ? getSlot(index)->getItem() : ItemInstance::EMPTY;
}

void ContainerMenu::moveItemStackTo(ItemInstance& item, int slotFrom, int slotTo, bool take)
{
    int index = slotFrom;
    if (take)
        index = slotTo - 1;

    if (item.isStackable())
    {
        while (item.m_count > 0 && ((!take && index < slotTo) || (take && index >= slotFrom)))
        {
            Slot* slot = getSlot(index);
            ItemInstance& slotItem = slot->getItem();
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
            ItemInstance& slotItem = slot->getItem();
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

ItemInstance ContainerMenu::clicked(int slotIndex, MouseButtonType mouseButton, bool quickMove, Player* player)
{
    ItemInstance result = ItemInstance::EMPTY;

    if (mouseButton != MOUSE_BUTTON_LEFT && mouseButton != MOUSE_BUTTON_RIGHT)
        return result;

    Inventory* inv = player->m_pInventory;

    if (slotIndex == -999)
    {
        if (!inv->getCarried().isEmpty())
        {
            if (mouseButton == MOUSE_BUTTON_LEFT)
            {
                player->drop(inv->getCarried());
                inv->setCarried(ItemInstance::EMPTY);
            }
            else if (mouseButton == MOUSE_BUTTON_RIGHT)
            {
                ItemInstance single = inv->getCarried().remove(1);
                player->drop(single);
                if (!inv->getCarried().m_count)
                    inv->setCarried(ItemInstance::EMPTY);
            }
        }
    }
    else
    {
        if (quickMove)
        {
            ItemInstance quickMoved = quickMoveStack(slotIndex);
            if (!quickMoved.isEmpty())
            {
                Slot* slot = getSlot(slotIndex);
                if (slot && slot->getItem() && slot->getItem().m_count < quickMoved.m_count)   
                    clicked(slotIndex, mouseButton, quickMove, player);
            }

            return result;
        }

        Slot* slot = getSlot(slotIndex);
        if (!slot)
            return result;

        slot->setChanged();
        ItemInstance slotItem = slot->getItem();

        ItemInstance& carried = inv->getCarried();

        if (slotItem.isEmpty() && carried.isEmpty())
            return result;

        int16_t count;
        if (!slotItem.isEmpty() && carried.isEmpty())
        {
            count = (mouseButton == MOUSE_BUTTON_LEFT) ? slotItem.m_count : (slotItem.m_count + 1) / 2;
            inv->setCarried(slot->remove(count));
            if (!slotItem.m_count)
                slot->set(ItemInstance::EMPTY);

            slot->onTake(inv->getCarried());
        }
        else if (slotItem.isEmpty() && !carried.isEmpty() && slot->mayPlace(carried))
        {
            count = (mouseButton == MOUSE_BUTTON_LEFT) ? carried.m_count : 1;
            if (count > slot->getMaxStackSize())
                count = slot->getMaxStackSize();

            slot->set(carried.remove(count));

            if (!carried.m_count)
                inv->setCarried(ItemInstance::EMPTY);
        }
        else if (!slotItem.isEmpty() && !carried.isEmpty())
        {
            if (slot->mayPlace(carried))
            {
                if (slotItem.getId() != carried.getId() || (slotItem.isStackedByData() && slotItem.getAuxValue() != carried.getAuxValue()))
                {
                    if (carried.m_count <= slot->getMaxStackSize())
                    {
                        slot->set(carried);
                        inv->setCarried(slotItem);
                    }
                }
                else if (slotItem.getId() == carried.getId())
                {
                    if (mouseButton == MOUSE_BUTTON_LEFT)
                    {
                        count = carried.m_count;
                        int space = slot->getMaxStackSize() - slotItem.m_count;
                        if (count > space) count = space;

                        int maxStack = carried.getMaxStackSize() - slotItem.m_count;
                        if (count > maxStack) count = maxStack;

                        carried.remove(count);
                        if (!carried.m_count)
                            inv->setCarried(ItemInstance::EMPTY);

                        slotItem.m_count += count;
                    }
                    else if (mouseButton == MOUSE_BUTTON_RIGHT)
                    {
                        count = 1;
                        int space = slot->getMaxStackSize() - slotItem.m_count;
                        if (count > space) count = space;

                        int maxStack = carried.getMaxStackSize() - slotItem.m_count;
                        if (count > maxStack) count = maxStack;

                        carried.remove(count);
                        if (carried.m_count == 0)
                            inv->setCarried(ItemInstance::EMPTY);

                        slotItem.m_count += count;
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
                            slot->set(ItemInstance::EMPTY);

                        slot->onTake(carried);
                    }
                }
            }
        }
    }

    //@Note: useless return value
    return result;
}

void ContainerMenu::setItem(int index, ItemInstance item)
{
    m_slots[index]->set(item);
}

void ContainerMenu::setAll(const std::vector<ItemInstance>& items)
{
    for (size_t i = 0; i < items.size(); ++i)
    {
        m_slots[i]->set(items[i]);
    }
}

void ContainerMenu::setData(int id, int value) {
}

uint16_t ContainerMenu::backup(Inventory*) {
    return ++m_changeUid;
}

void ContainerMenu::deleteBackup(uint16_t) {
}

void ContainerMenu::rollbackToBackup(uint16_t) {
}

bool ContainerMenu::isSynched(Player* player) const
{
    return unsynchedPlayers.find(player) == unsynchedPlayers.end();
}

void ContainerMenu::setSynched(Player* player, bool isSynched)
{
    if (isSynched)
        unsynchedPlayers.erase(player);
    else
        unsynchedPlayers.insert(player);
}
