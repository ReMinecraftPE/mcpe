#include "CompoundContainer.hpp"

class CompoundContainer::ChildListener : public ContainerContentChangeListener
{
public:
    ChildListener(CompoundContainer* owner, int offset)
        : m_pOwner(owner), m_offset(offset)
    {
    }

    void containerContentChanged(Container* container, SlotID slot) override
    {
        if (m_pOwner)
            m_pOwner->setContainerChanged(slot + m_offset);
    }

private:
    CompoundContainer* m_pOwner;
    int m_offset;
};

CompoundContainer::CompoundContainer(const std::string& name, Container* c1, Container* c2) :
    m_name(name), m_pLeftContainer(c1), m_pRightContainer(c2), m_pLeftListener(nullptr), m_pRightListener(nullptr)
{
    m_pLeftListener = new ChildListener(this, 0);
    m_pRightListener = new ChildListener(this, m_pLeftContainer->getContainerSize());

    if (m_pLeftContainer)
        m_pLeftContainer->addContentChangeListener(m_pLeftListener);
    if (m_pRightContainer)
        m_pRightContainer->addContentChangeListener(m_pRightListener);
}

CompoundContainer::~CompoundContainer()
{
    if (m_pLeftContainer && m_pLeftListener)
        m_pLeftContainer->removeContentChangeListener(m_pLeftListener);
    if (m_pRightContainer && m_pRightListener)
        m_pRightContainer->removeContentChangeListener(m_pRightListener);

    delete m_pLeftListener;
    delete m_pRightListener;
}

uint16_t CompoundContainer::getContainerSize() const
{
    return uint16_t(m_pLeftContainer->getContainerSize() + m_pRightContainer->getContainerSize());
}

std::string CompoundContainer::getName() const
{
    return m_name;
}

ItemStack& CompoundContainer::getItem(int index)
{
    if (index >= m_pLeftContainer->getContainerSize())
        return m_pRightContainer->getItem(index - m_pLeftContainer->getContainerSize());
    else
        return m_pLeftContainer->getItem(index);
}

ItemStack CompoundContainer::removeItem(int index, int count)
{
    if (index >= m_pLeftContainer->getContainerSize())
        return m_pRightContainer->removeItem(index - m_pLeftContainer->getContainerSize(), count);
    else
        return m_pLeftContainer->removeItem(index, count);
}

void CompoundContainer::setItem(int index, const ItemStack& item)
{
    if (index >= m_pLeftContainer->getContainerSize())
        m_pRightContainer->setItem(index - m_pLeftContainer->getContainerSize(), item);
    else
        m_pLeftContainer->setItem(index, item);
}

int CompoundContainer::getMaxStackSize()
{
    return m_pLeftContainer->getMaxStackSize();
}

void CompoundContainer::setContainerChanged(SlotID slot)
{
    for (ContentChangeListeners::iterator it = m_contentChangeListeners.begin(); it != m_contentChangeListeners.end(); ++it)
    {
        ContainerContentChangeListener* listener = *it;
        listener->containerContentChanged(this, slot);
    }
}

bool CompoundContainer::stillValid(Player* player) const
{
    return m_pLeftContainer->stillValid(player) && m_pRightContainer->stillValid(player);
}

void CompoundContainer::addContentChangeListener(ContainerContentChangeListener* listener)
{
    m_contentChangeListeners.insert(listener);
}

void CompoundContainer::removeContentChangeListener(ContainerContentChangeListener* listener)
{
    m_contentChangeListeners.erase(listener);
}

