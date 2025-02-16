#include "SynchedEntityData.hpp"

#define MAP(cType, typeEnum, value) template<> SynchedEntityData::DataType SynchedEntityData::DataTypeMap::typeFor<cType>() { return SynchedEntityData::typeEnum; } \
		template<> cType SynchedEntityData::DataTypeMap::defaultValueFor<cType>() { return value; }

MAP(int8_t,       TYPE_INT8,         0)
MAP(int16_t,      TYPE_INT16,        0)
MAP(int32_t,      TYPE_INT32,        0)
MAP(float,        TYPE_FLOAT,        0.0f) // float because VS2010 was bitching about float_t not existing despite including math.h or cmath
MAP(std::string,  TYPE_STRING,       Util::EMPTY_STRING)
MAP(ItemInstance, TYPE_ITEMINSTANCE, ItemInstance())
MAP(TilePos,      TYPE_TILEPOS,      TilePos())
MAP(int64_t,      TYPE_INT64,        0)
MAP(Vec3,         TYPE_VEC3,         Vec3())

#undef MAP

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

void SynchedEntityData::_resizeToContain(DataID id)
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
