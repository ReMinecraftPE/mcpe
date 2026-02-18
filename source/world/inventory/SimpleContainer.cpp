#include "SimpleContainer.hpp"
#include "ContainerContentChangeListener.hpp"
#include "ContainerSizeChangeListener.hpp"

SimpleContainer::SimpleContainer(int size, const std::string& name)
    : m_items(size)
    , m_name(name)
{
}

uint16_t SimpleContainer::getContainerSize() const
{
    return uint16_t(m_items.size());
}

ItemStack& SimpleContainer::getItem(int index) 
{
    return m_items[index];
}

ItemStack SimpleContainer::removeItem(int index, int count)
{
    if (!m_items[index].isEmpty())
    {
        ItemStack result = ItemStack::EMPTY;
        if (m_items[index].m_count <= count)
        {
            result = m_items[index];
            m_items[index] = ItemStack::EMPTY;
            setContainerChanged(index);
            return result;
        }
        else
        {
            result = m_items[index].remove(count);
            if (!m_items[index].m_count)
                m_items[index] = ItemStack::EMPTY;

            setContainerChanged(index);
            return result;
        }
    }
    return ItemStack::EMPTY;
}

void SimpleContainer::setItem(int index, const ItemStack& item)
{
    m_items[index] = item;
    if (!item.isEmpty() && item.m_count > getMaxStackSize())
        m_items[index].m_count = getMaxStackSize();

    setContainerChanged(index);
}

std::string SimpleContainer::getName() const
{
    return m_name;
}

void SimpleContainer::setContainerChanged(SlotID slot)
{
    for (ContentChangeListeners::iterator it = m_contentChangeListeners.begin(); it != m_contentChangeListeners.end(); it++)
    {
        ContainerContentChangeListener* pListener = *it;
        pListener->containerContentChanged(slot);
    }
}

bool SimpleContainer::stillValid(Player* player) const
{
    return true;
}

void SimpleContainer::addContentChangeListener(ContainerContentChangeListener* listener)
{
    m_contentChangeListeners.insert(listener);
}

void SimpleContainer::addSizeChangeListener(ContainerSizeChangeListener* listener)
{
    m_sizeChangeListeners.insert(listener);
}

void SimpleContainer::removeContentChangeListener(ContainerContentChangeListener* listener)
{
    m_contentChangeListeners.erase(listener);
}

void SimpleContainer::removeSizeChangeListener(ContainerSizeChangeListener* listener)
{
    m_sizeChangeListeners.erase(listener);
}

void SimpleContainer::clear()
{
    std::fill(m_items.begin(), m_items.end(), ItemStack::EMPTY);
}

void SimpleContainer::load(const CompoundTag& tag)
{
    clear();
    const ListTag* list = tag.getList("Items");

    const std::vector<Tag*>& itemTags = list->rawView();

    for (std::vector<Tag*>::const_iterator it = itemTags.begin(); it != itemTags.end(); it++)
    {
        const CompoundTag* itemTag = (const CompoundTag*)*it;
        if (itemTag)
        {
            uint8_t slot = itemTag->getInt8("Slot") & 255;
            ItemStack item = ItemStack::fromTag(*itemTag);
            if (!itemTag->isEmpty() && slot >= 0 && slot < m_items.size())
                m_items[slot] = item;
        }
    }
    
}

void SimpleContainer::save(CompoundTag& tag) const
{
    ListTag* list = new ListTag;

    for (size_t i = 0; i < m_items.size(); ++i)
    {
        if (!m_items[i].isEmpty())
        {
            CompoundTag* itemTag = new CompoundTag;
            itemTag->putInt8("Slot", i);
            m_items[i].save(*itemTag);
            list->add(itemTag);
        }
    }

    tag.put("Items", list);
}
