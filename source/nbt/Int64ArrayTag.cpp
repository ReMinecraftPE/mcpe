#include <stdint.h>
#include <assert.h>
#include <sstream>

#include "Int64ArrayTag.hpp"

#define MAX_SIZE 4096 / 2
#define ELEMENT_SIZE sizeof(int64_t)

Int64ArrayTag::Int64ArrayTag() : m_data() {}
Int64ArrayTag::Int64ArrayTag(const TagMemoryChunk& data)
	: m_data(data.copy())
{
}

void Int64ArrayTag::write(IDataOutput& dos) const
{
	dos.writeInt32(m_data.m_elements);
	for (uint32_t i = 0; i < m_data.m_elements; i++)
	{
		dos.writeInt64(m_data.m_buffer[ELEMENT_SIZE * i]);
	}
}

void Int64ArrayTag::load(IDataInput& dis)
{
	int32_t elements = dis.readInt32();
	if (elements <= 0 )
		return;

	uint32_t size = MAX_SIZE;
	if (elements < MAX_SIZE)
		size = elements;
	uint8_t* buf = m_data.alloc<int64_t>(size);
	if (buf)
	{
		for (uint32_t i = 0; i < elements; i++)
		{
			if (dis.numBytesLeft() == 0)
				break;
			if (i >= m_data.m_elements)
			{
				size = m_data.m_elements + MAX_SIZE;
				if (elements < size)
					size = elements;
				m_data.alloc<int64_t>(size);
			}
			// @HACK
			((int64_t*)buf)[i] = dis.readInt64();
		}
	}
	else
	{
		assert(!"Memory allocation failed");
	}
}

std::string Int64ArrayTag::toString() const
{
	std::ostringstream sstream("[");
	sstream << m_data.m_elements << " 64-bit ints]";
	return sstream.str();
}

Tag* Int64ArrayTag::copy() const
{
	return new Int64ArrayTag(m_data);
}

bool Int64ArrayTag::operator==(const Tag& other) const
{
	const Int64ArrayTag& other2 = (const Int64ArrayTag&)(other);

	if (getId() != other2.getId())
		return false;

	if (other2.m_data != m_data)
		return false;

	return true;
}