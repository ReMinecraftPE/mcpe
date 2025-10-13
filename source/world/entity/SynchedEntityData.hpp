#pragma once

#include <vector>
#include <string>

#include "common/Util.hpp"
#include "common/DataIO.hpp"
#include "world/item/ItemInstance.hpp"
#include "world/phys/Vec3.hpp"

#define C_ENTITYDATA_EOF_MARKER (127)
#define C_ENTITYDATA_TYPE_MASK (224)
#define C_ENTITYDATA_TYPE_SHIFT (5)
#define C_ENTITYDATA_MAX_ID_VALUE (31)

class SynchedEntityData
{
public:
	typedef uint16_t DataID;

	// Order here matters!
	enum DataType
	{
		TYPE_INT8,
		TYPE_INT16,
		TYPE_INT32,
		TYPE_FLOAT,
		TYPE_STRING,
		TYPE_ITEMINSTANCE,
		TYPE_COMPOUNDTAG = TYPE_ITEMINSTANCE,
		TYPE_TILEPOS,
		TYPE_INT64,
		TYPE_VEC3
	};

	class DataTypeMap
	{
	public:
		template<typename T>
		static DataType typeFor();

		template<typename T>
		static T defaultValueFor();
	};

	class DataItem
	{
	private:
		DataType m_type;
		DataID m_id;
		bool m_bDirty;

	public:
		DataItem(DataType type, DataID id)
		{
			m_id = id;
			m_type = type;
			m_bDirty = true;
		}

		DataID getId() const { return m_id; }
		DataType getType() const { return m_type; }
		bool isDirty() const { return m_bDirty; }
		void setDirty(bool dirty) { m_bDirty = dirty; }

		// beautiful
		template<typename T>
		T getData() const { return ((DataItem2<T>*)this)->getData(); }
	};

	template<typename T>
	class DataItem2 : public DataItem
	{
	private:
		T m_data;

	public:
		DataItem2(DataType type, DataID id, T data)
			: DataItem(type, id)
		{
			m_data = data;
		}

		void setData(T data) { m_data = data; }
		T getData() const { return m_data; }
	};

	typedef std::vector<DataItem*> ItemsArray;

public:
	SynchedEntityData();
	~SynchedEntityData();

private:
	void _updateMinMaxIdxDirty(DataID id);
	DataItem* _get(DataID id) const;
	void _resizeToContain(DataID id);

public:
	bool hasData(DataID id) const;
	bool isDirty() const;
	void clear();
	ItemsArray packDirty();
	void packAll(IDataOutput& dos) const;
	void assignValues(const ItemsArray& items);

	// These all need to be defined in the header file per https://stackoverflow.com/questions/456713/why-do-i-get-unresolved-external-symbol-errors-when-using-templates
	template<typename T> void define(DataID id, const T& value)
	{
		// Data value id is too big with %d! (Max is %d )
		assert(id <= C_ENTITYDATA_MAX_ID_VALUE);

		// Duplicate id value for %d!
		assert(id >= m_itemsArray.size() || m_itemsArray[id] == nullptr);

		_resizeToContain(id);

		DataItem* dataItem = new DataItem2<T>(DataTypeMap::typeFor<T>(), id, value);
		m_itemsArray[id] = dataItem;

		_updateMinMaxIdxDirty(id);
	}

	template<typename T> T get(DataID id) const
	{
		if (hasData(id) && m_itemsArray[id]->getType() == DataTypeMap::typeFor<T>())
		{
			return m_itemsArray[id]->getData<T>();
		}
		else
		{
			return DataTypeMap::defaultValueFor<T>();
		}
	}

	template<typename T> bool set(DataItem2<T>* dataItem, const T& value)
	{
		if (dataItem == nullptr)
		{
			// DataItem not found!
			return false;
		}

		if (dataItem->getType() == DataTypeMap::typeFor<T>())
		{
			if (value == dataItem->getData())
			{
				return false;
			}
			dataItem->setData(value);
			dataItem->setDirty(true);

			_updateMinMaxIdxDirty(dataItem->getId());

			return true;
		}
		else
		{
			assert(!"Wrong value type for this ID!");
			return false;
		}
	}

	template<typename T> bool set(DataID id, const T& value)
	{
		DataItem2<T>* dataItem = (DataItem2<T>*)_get(id);
		return set(dataItem, value);
	}

private:
	static void _WriteDataItem(IDataOutput& dos, const DataItem& dataItem);

public:
	static void Pack(const ItemsArray& items, IDataOutput& dos);
	static ItemsArray Unpack(IDataInput& dis);

private:
	ItemsArray m_itemsArray;
	DataID m_minIdxDirty;
	DataID m_maxIdxDirty;
};
