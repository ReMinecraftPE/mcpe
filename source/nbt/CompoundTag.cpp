#include <assert.h>
#include <sstream>

#include "CompoundTag.hpp"
#include "common/Util.hpp"
#include "common/Logger.hpp"

CompoundTag::CompoundTag()
{
    m_bLeak = false;
}

void CompoundTag::write(IDataOutput& dos) const
{
    for (NamedTagMap::ConstIterator it = m_tags.begin(); it != m_tags.end(); it++)
	{
		writeNamedTag(it.key(), *it.value(), dos);
    }

    dos.writeInt8(TAG_TYPE_END);
}

void CompoundTag::load(IDataInput& dis)
{
	std::string name;

    while (true)
	{
		if (dis.numBytesLeft() == 0)
			break;

        Tag* tag = readNamedTag(dis, name);

        if (!tag)
		{
			// We want this to shit itself when in debug mode so we know something's fucked
			assert(!"Null tag returned in CompoundTag::read");
			break;
		}

        if (tag->getId() == TAG_TYPE_END)
        {
            delete tag;
            break;
        }

        m_tags[name] = tag;
    }
}

void CompoundTag::put(const std::string& name, Tag* tag)
{
	// @TODO: is tag managed by something already? is it our job to deallocate it? no clue.
	// for now, let's manage the memory, and just see what happens
	if (!tag)
	{
		assert(!"Cannot store null tags");
		return;
	}

	if (!m_bLeak)
	{
		Tag* oldTag = m_tags[name];
		if (oldTag)
		{
			oldTag->deleteChildren();
			delete oldTag;
		}
	}

	m_tags[name] = tag;
}

void CompoundTag::putInt8(const std::string& name, int8_t value)
{
    put(name, new Int8Tag(value));
}

void CompoundTag::putInt16(const std::string& name, int16_t value)
{
    put(name, new Int16Tag(value));
}

void CompoundTag::putInt32(const std::string& name, int32_t value)
{
    put(name, new Int32Tag(value));
}

void CompoundTag::putInt64(const std::string& name, int64_t value)
{
    put(name, new Int64Tag(value));
}

void CompoundTag::putFloat(const std::string& name, float value)
{
    put(name, new FloatTag(value));
}

void CompoundTag::putDouble(const std::string& name, double value)
{
    put(name, new DoubleTag(value));
}

void CompoundTag::putString(const std::string& name, const std::string& value)
{
    put(name, new StringTag(value));
}

void CompoundTag::putInt8Array(const std::string& name, const TagMemoryChunk& mem)
{
    put(name, new Int8ArrayTag(mem));
}

void CompoundTag::putInt32Array(const std::string& name, const TagMemoryChunk& mem)
{
    put(name, new Int32ArrayTag(mem));
}

void CompoundTag::putInt64Array(const std::string& name, const TagMemoryChunk& mem)
{
    put(name, new Int64ArrayTag(mem));
}

void CompoundTag::putCompound(const std::string& name, CompoundTag* value)
{
	put(name, (Tag*)value);
}

void CompoundTag::putBoolean(const std::string& name, bool value)
{
    putInt8(name, value);
}

bool CompoundTag::contains(const std::string& name) const
{
    return m_tags.find(name) != m_tags.end();
}

bool CompoundTag::contains(const std::string& name, Tag::Type type) const
{
	// @TODO: inefficient, all get functions check this, then get the Tag for the second time
	const Tag* tag = get(name);
	return tag && tag->getId() == type;
}

const Tag* CompoundTag::get(const std::string& name) const
{
	NamedTagMap::ConstIterator it = m_tags.find(name);
	if (it != m_tags.end()) return it.value();
    return nullptr;
}

Tag* CompoundTag::get(const std::string& name)
{
	NamedTagMap::Iterator it = m_tags.find(name);
	if (it != m_tags.end()) return it.value();
	return nullptr;
}

int8_t CompoundTag::getInt8(const std::string& name) const
{
	if (contains(name, TAG_TYPE_INT8))
		return ((const Int8Tag*)get(name))->m_data;

	return 0;
}

int16_t CompoundTag::getInt16(const std::string& name) const
{
	if (contains(name, TAG_TYPE_INT16))
		return ((const Int16Tag*)get(name))->m_data;

	return 0;
}

int32_t CompoundTag::getInt32(const std::string& name) const
{
	if (contains(name, TAG_TYPE_INT32))
		return ((const Int32Tag*)get(name))->m_data;

	return 0;
}

int64_t CompoundTag::getInt64(const std::string& name) const
{
	if (contains(name, TAG_TYPE_INT64))
		return ((const Int64Tag*)get(name))->m_data;

	return 0;
}

float CompoundTag::getFloat(const std::string& name) const
{
	if (contains(name, TAG_TYPE_FLOAT))
		return ((const FloatTag*)get(name))->m_data;

	return 0.0f;
}

double CompoundTag::getDouble(const std::string& name) const
{
	if (contains(name, TAG_TYPE_DOUBLE))
		return ((const DoubleTag*)get(name))->m_data;

	return 0.0;
}

const std::string& CompoundTag::getString(const std::string& name) const
{
	if (contains(name, TAG_TYPE_STRING))
		return ((const StringTag*)get(name))->m_data;

	return Util::EMPTY_STRING;
}

const TagMemoryChunk* CompoundTag::getInt8Array(const std::string& name) const
{
	if (contains(name, TAG_TYPE_INT8_ARRAY))
	{
		return &((const Int8ArrayTag*)get(name))->m_data;
	}

	return nullptr;
}

const TagMemoryChunk* CompoundTag::getInt32Array(const std::string& name) const
{
	if (contains(name, TAG_TYPE_INT32_ARRAY))
	{
		return &((const Int32ArrayTag*)get(name))->m_data;
	}

	return nullptr;
}

const TagMemoryChunk* CompoundTag::getInt64Array(const std::string& name) const
{
	if (contains(name, TAG_TYPE_INT64_ARRAY))
	{
		return &((const Int64ArrayTag*)get(name))->m_data;
	}

	return nullptr;
}

const CompoundTag* CompoundTag::getCompound(const std::string& name) const
{
	if (contains(name, TAG_TYPE_COMPOUND))
	{
		return (const CompoundTag*)get(name);
	}

	return nullptr;
}

CompoundTag* CompoundTag::getCompound(const std::string& name)
{
	if (contains(name, TAG_TYPE_COMPOUND))
	{
		return (CompoundTag*)get(name);
	}

	return nullptr;
}

const ListTag* CompoundTag::getList(const std::string& name) const
{
	if (contains(name, TAG_TYPE_LIST))
	{
		return (const ListTag*)get(name);
	}

	return nullptr;
}

bool CompoundTag::getBoolean(const std::string& name) const
{
    return getInt8(name) != 0;
}

std::string CompoundTag::toString() const
{
	std::ostringstream sstream;
	sstream << m_tags.size() << " entries";
	return sstream.str();
}

CompoundTag* CompoundTag::copy() const
{
	return new CompoundTag(*this);
}

CompoundTag CompoundTag::clone() const
{
	return CompoundTag(*this);
}

CompoundTag* CompoundTag::uniqueClone() const
{
	CompoundTag* newTag = new CompoundTag();

	for (NamedTagMap::ConstIterator it = m_tags.begin(); it != m_tags.end(); it++)
	{
		newTag->put(it.key(), it.value()->copy());
	}

	return newTag;
}

bool CompoundTag::remove(const std::string& name)
{
	NamedTagMap::Iterator it = m_tags.find(name);
	if (it == m_tags.end())
		return false;

	delete it.value();
	m_tags.remove(name);

	return true;
}

void CompoundTag::deleteChildren()
{
    if (!m_bLeak)
    {
        for (NamedTagMap::Iterator it = m_tags.begin(); it != m_tags.end(); it++)
        {
            Tag* tag = it.value();
            tag->deleteChildren();
            delete tag;
        }
    }

	m_tags.clear();
}

bool CompoundTag::operator==(const Tag& other) const
{
	const CompoundTag& other2 = (const CompoundTag&)(other);
	if (getId() == other2.getId() && m_tags.size() == other2.m_tags.size())
	{
		for (NamedTagMap::ConstIterator it = m_tags.begin(); it != m_tags.end(); it++)
		{
			NamedTagMap::ConstIterator it2 = other2.m_tags.find(it.key());
			if (it2 == other2.m_tags.end())
				return false; // Failed to find tag in other by name

			if (it.value() != it2.value())
				return false;
		}

		return true;
	}

	return false;
}
