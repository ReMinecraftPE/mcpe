#include "SynchedEntityData.hpp"

SynchedEntityData::SynchedEntityData()
{
    m_itemsArray = ItemsArray();
    m_minIdxDirty = -1;
    m_maxIdxDirty = 0;
}

void SynchedEntityData::_updateMinMaxIdxDirty(DataID id)
{
    if (id < m_minIdxDirty)
        m_minIdxDirty = id;

    if (id > m_maxIdxDirty)
        m_maxIdxDirty = id;
}

SynchedEntityData::DataItem* SynchedEntityData::_get(DataID id) const
{
    // Data item not found
    assert(hasData(id));
    return m_itemsArray[id];
}

void SynchedEntityData::_resizeToContain(SynchedEntityData::DataID id)
{
    for (int i = m_itemsArray.size(); i <= id; i++)
    {
        m_itemsArray.push_back(nullptr);
    }
}

bool SynchedEntityData::hasData(DataID id) const
{
    return id <= m_itemsArray.size() && m_itemsArray[id] != nullptr;
}
