#pragma once

#include <string>
#include <cstring>
#include <stdint.h>

#include "common/DataIO.hpp"

class Tag
{
public:
	enum Type
	{
		TAG_TYPE_END,
		TAG_TYPE_INT8,
		TAG_TYPE_INT16,
		TAG_TYPE_INT32,
		TAG_TYPE_INT64,
		TAG_TYPE_FLOAT,
		TAG_TYPE_DOUBLE,
		TAG_TYPE_INT8_ARRAY,
		TAG_TYPE_STRING,
		TAG_TYPE_LIST,
		TAG_TYPE_COMPOUND,
		TAG_TYPE_INT32_ARRAY,
		TAG_TYPE_INT64_ARRAY
	};

public:
	virtual ~Tag() {}

public:
	virtual void deleteChildren() {};
    virtual void write(IDataOutput& dos) const = 0;
    virtual void load(IDataInput& dis) = 0;
	virtual std::string toString() const = 0;
    virtual Tag::Type getId() const = 0;
	//virtual bool equals(const Tag& other) = 0;
	virtual Tag* copy() const = 0;
	//virtual uint64_t hash() = 0;

public:
	virtual bool operator==(const Tag& other) const = 0;

public:
	static std::string getTagName(Tag::Type type);
    static Tag* newTag(Tag::Type type);
    static Tag* readNamedTag(IDataInput& dis, std::string& name);
    static void writeNamedTag(const std::string& name, const Tag& tag, IDataOutput& dos);
};

class TagMemoryChunk
{
private:
	template <typename T>
	uint8_t* _alloc(unsigned int count, unsigned int size)
	{
		m_elements = count;
		m_size = size;
		if (size > 0)
		{
			m_buffer = new uint8_t(size);
			if (m_buffer)
				memset(m_buffer, 0, size);
		}

		return m_buffer;
	}
public:
	TagMemoryChunk();
	~TagMemoryChunk();

	template <typename T>
	uint8_t* alloc(unsigned int count)
	{
		return _alloc<T>(count, sizeof(T) * count);
	}

	TagMemoryChunk copy() const;

	bool operator!=(const TagMemoryChunk& other) const;
public:
	unsigned int m_elements;
	unsigned int m_size;
	uint8_t* m_buffer;
};