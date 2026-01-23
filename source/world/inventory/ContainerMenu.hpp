#pragma once

#include <vector>
#include <set>
#include "world/item/ItemInstance.hpp"
#include "client/player/input/MouseDevice.hpp"

class Player;
class Inventory;
class Slot;
class Container;
class ContainerListener;

class ContainerMenu
{
protected:
    std::vector<ItemInstance> m_lastSlots;
    uint16_t m_changeUid;
    std::vector<ContainerListener*> m_listeners;
    std::set<Player*> unsynchedPlayers;

public:
    ContainerMenu();

    int m_containerId;
    std::vector<Slot*> m_slots;
    virtual ~ContainerMenu();

    void addSlot(Slot* slot);
    virtual void addSlotListener(ContainerListener* listener);
    void sendData(int id, int value);
    virtual void broadcastChanges();
    virtual void removed(Player* player);
    virtual void slotsChanged(Container* container);

    std::vector<ItemInstance> getItems();
    Slot* getSlotFor(Container* container, int index);
    Slot* getSlot(int index);
    virtual ItemInstance clicked(int slotIndex, MouseButtonType mouseButton, bool quickMove, Player* player);
    virtual ItemInstance quickMoveStack(int index);
    virtual void moveItemStackTo(ItemInstance& item, int slotFrom, int slotTo, bool take);

    void setItem(int slotIndex, ItemInstance item);
    void setAll(const std::vector<ItemInstance>& items);
    virtual void setData(int id, int value);

    uint16_t backup(Inventory* inventory);
    void deleteBackup(uint16_t id);
    void rollbackToBackup(uint16_t id);

    bool isSynched(Player* player) const;
    void setSynched(Player* player, bool isSynched);

    virtual bool stillValid(Player* player) const = 0;

    //Unused
    virtual bool isPauseScreen() const { return false; }
};
