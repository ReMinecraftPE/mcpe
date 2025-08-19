#include <stdint.h>
#include <assert.h>
#include <sstream>

#include "Int32ArrayTag.hpp"

#define MAX_SIZE 4096
#define ELEMENT_SIZE sizeof(int32_t)

Int32ArrayTag::Int32ArrayTag() : m_data() {}
Int32ArrayTag::Int32ArrayTag(const TagMemoryChunk& data)
	: m_data(data.copy())
{
}

void Int32ArrayTag::write(IDataOutput& dos) const
{
	dos.writeInt32(m_data.m_elements);
	for (uint32_t i = 0; i < m_data.m_elements; i++)
	{
		dos.writeInt32(m_data.m_buffer[ELEMENT_SIZE * i]);
	}
}

void Int32ArrayTag::load(IDataInput& dis)
{
	int32_t elements = dis.readInt32();
	if (elements <= 0 )
		return;

	uint32_t size = MAX_SIZE;
	if (elements < MAX_SIZE)
		size = elements;
	uint8_t* buf = m_data.alloc<int32_t>(size);
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
				m_data.alloc<int32_t>(size);
			}
			// @HACK
			((int32_t*)buf)[i] = dis.readInt32();
		}
	}
	else
	{
		assert(!"Memory allocation failed");
	}
}

std::string Int32ArrayTag::toString() const
{
	std::ostringstream sstream("[");
	sstream << m_data.m_elements << " 32-bit ints]";
	return sstream.str();
}

Tag* Int32ArrayTag::copy() const
{
	return new Int32ArrayTag(m_data);
}

bool Int32ArrayTag::operator==(const Tag& other) const
{
	const Int32ArrayTag& other2 = (const Int32ArrayTag&)(other);

	if (getId() != other2.getId())
		return false;

	if (other2.m_data != m_data)
		return false;

	return true;
}