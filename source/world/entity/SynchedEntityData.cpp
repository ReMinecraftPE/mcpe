#include <limits.h>

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
    m_minIdxDirty = INT_MAX;
    m_maxIdxDirty = 0;
}

SynchedEntityData::~SynchedEntityData()
{
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

bool SynchedEntityData::isDirty() const
{
    return m_minIdxDirty != (DataID)INT_MAX;
}

void SynchedEntityData::clear()
{
    for (int i = 0; i < m_itemsArray.size(); i++)
    {
        DataItem* item = m_itemsArray[i];
        SAFE_DELETE(item);
    }

    m_itemsArray.clear();
    // Mark as clean
    m_minIdxDirty = INT_MAX;
    m_maxIdxDirty = 0;
}

SynchedEntityData::ItemsArray SynchedEntityData::packDirty()
{
    ItemsArray result;

    for (int i = m_minIdxDirty; i <= m_maxIdxDirty; i++)
    {
        DataItem* item = m_itemsArray[i];
        if (item && item->isDirty())
        {
            item->setDirty(false);
            result.push_back(item);
        }
    }

    // Mark as clean
    m_minIdxDirty = INT_MAX;
    return result;
}

void SynchedEntityData::packAll(IDataOutput& dos) const
{
    Pack(m_itemsArray, dos);
}

void SynchedEntityData::assignValues(const ItemsArray& items)
{
    for (int i = 0; i < items.size(); i++)
    {
        DataItem* newItem = items[i];
        DataID itemId = newItem->getId();
        if (itemId >= m_itemsArray.size())
            continue;

        DataItem* oldItem = m_itemsArray[itemId];
        if (!oldItem)
            continue;

        bool result = false;

        // ugly
        switch (newItem->getType())
        {
        case TYPE_INT8:
            result = set<int8_t>(itemId, newItem->getData<int8_t>());
            break;
        case TYPE_INT16:
            result = set<int16_t>(itemId, newItem->getData<int16_t>());
            break;
        case TYPE_INT32:
            result = set<int32_t>(itemId, newItem->getData<int32_t>());
            break;
        case TYPE_FLOAT:
            result = set<float>(itemId, newItem->getData<float>());
            break;
        case TYPE_STRING:
            result = set<std::string>(itemId, newItem->getData<std::string>());
            break;
        case TYPE_ITEMINSTANCE:
            result = set<ItemInstance>(itemId, newItem->getData<ItemInstance>());
            break;
        case TYPE_TILEPOS:
            result = set<TilePos>(itemId, newItem->getData<TilePos>());
            break;
        case TYPE_INT64:
            result = set<int64_t>(itemId, newItem->getData<int64_t>());
            break;
        case TYPE_VEC3:
            result = set<Vec3>(itemId, newItem->getData<Vec3>());
            break;
        default:
            continue;
        }
    }
}

void SynchedEntityData::_WriteDataItem(IDataOutput& dos, const DataItem& dataItem)
{
    int8_t var2 = dataItem.getType() << C_ENTITYDATA_TYPE_SHIFT | dataItem.getId() & C_ENTITYDATA_MAX_ID_VALUE;
    dos.writeInt8(var2);
    switch (dataItem.getType())
    {
        case TYPE_INT8:
            dos.writeInt8(dataItem.getData<int8_t>());
            break;
        case TYPE_INT16:
            dos.writeInt16(dataItem.getData<int16_t>());
            break;
        case TYPE_INT32:
            dos.writeInt32(dataItem.getData<int32_t>());
            break;
        case TYPE_FLOAT:
            dos.writeFloat(dataItem.getData<float>());
            break;
        case TYPE_STRING:
            dos.writeString(dataItem.getData<std::string>());
            break;
        case TYPE_ITEMINSTANCE:
        {
            ItemInstance item = dataItem.getData<ItemInstance>();
            dos.writeInt16(item.getItem()->m_itemID);
            dos.writeInt8(item.m_count);
            dos.writeInt16(item.getAuxValue());
            break;
        }
        case TYPE_TILEPOS:
        {
            TilePos tilePos = dataItem.getData<TilePos>();
            dos.writeInt32(tilePos.x);
            dos.writeInt32(tilePos.y);
            dos.writeInt32(tilePos.z);
            break;
        }
        case TYPE_INT64:
            dos.writeInt64(dataItem.getData<int64_t>());
            break;
        case TYPE_VEC3:
        {
            Vec3 pos = dataItem.getData<Vec3>();
            dos.writeFloat(pos.x);
            dos.writeFloat(pos.y);
            dos.writeFloat(pos.z);
            break;
        }
    }
}

void SynchedEntityData::Pack(const ItemsArray& items, IDataOutput& dos)
{
    for (int i = 0; i < items.size(); i++)
    {
        const DataItem* item = items[i];
        if (item)
            _WriteDataItem(dos, *item);
    }

    dos.writeInt8(C_ENTITYDATA_EOF_MARKER);
}

SynchedEntityData::ItemsArray SynchedEntityData::Unpack(IDataInput& dis)
{
    ItemsArray result;

    for (int8_t var2 = dis.readInt8(); var2 != C_ENTITYDATA_EOF_MARKER; var2 = dis.readInt8())
    {
        DataType dataType = (DataType)(var2 >> 5);
        DataID dataId = var2 & C_ENTITYDATA_MAX_ID_VALUE;
        DataItem* dataItem = nullptr;

        switch (dataType)
        {
            case TYPE_INT8:
                dataItem = new DataItem2<int8_t>(dataType, dataId, dis.readInt8());
                break;
            case TYPE_INT16:
                dataItem = new DataItem2<int16_t>(dataType, dataId, dis.readInt16());
                break;
            case TYPE_INT32:
                dataItem = new DataItem2<int32_t>(dataType, dataId, dis.readInt32());
                break;
            case TYPE_FLOAT:
                dataItem = new DataItem2<float>(dataType, dataId, dis.readFloat());
                break;
            case TYPE_STRING:
                dataItem = new DataItem2<std::string>(dataType, dataId, dis.readString());
                break;
            case TYPE_ITEMINSTANCE:
            {
                int16_t itemId = dis.readInt16();
                int8_t amount = dis.readInt8();
                int16_t auxValue = dis.readInt16();
                dataItem = new DataItem2<ItemInstance>(dataType, dataId, ItemInstance(itemId, amount, auxValue));
                break;
            }
            case TYPE_TILEPOS:
            {
                int32_t x = dis.readInt32();
                int32_t y = dis.readInt32();
                int32_t z = dis.readInt32();
                dataItem = new DataItem2<TilePos>(dataType, dataId, TilePos(x, y, z));
                break;
            }
            case TYPE_INT64:
                dataItem = new DataItem2<int64_t>(dataType, dataId, dis.readInt64());
                break;
            case TYPE_VEC3:
            {
                float x = dis.readFloat();
                float y = dis.readFloat();
                float z = dis.readFloat();
                dataItem = new DataItem2<Vec3>(dataType, dataId, Vec3(x, y, z));
                break;
            }
        }

        result.push_back(dataItem);
    }

    return result;
}