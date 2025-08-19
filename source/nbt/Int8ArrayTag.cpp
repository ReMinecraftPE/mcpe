#include <sstream>

#include "Int8ArrayTag.hpp"

Int8ArrayTag::Int8ArrayTag() {}
Int8ArrayTag::Int8ArrayTag(const TagMemoryChunk& data)
	: m_data(data.copy())
{
}

void Int8ArrayTag::write(IDataOutput& dos) const
{
    dos.writeInt32(m_data.m_elements);
    dos.writeBytes(m_data.m_buffer, m_data.m_elements);
}

void Int8ArrayTag::load(IDataInput& dis)
{
	int32_t elements = dis.readInt32();
	m_data.m_elements = elements;

	if (elements <= 0)
		return;

	unsigned int bytesLeft = dis.numBytesLeft();
	int32_t count = elements;
	if (bytesLeft < elements)
		count = bytesLeft;
	void* buf = m_data.alloc<int8_t>(count);
	dis.readBytes(buf, m_data.m_elements);
}

std::string Int8ArrayTag::toString() const
{
	std::ostringstream sstream("[");
	sstream << m_data.m_elements << " 8-bit ints]";
	return sstream.str();
}

Tag* Int8ArrayTag::copy() const
{
	return new Int8ArrayTag(m_data);
}

bool Int8ArrayTag::operator==(const Tag& other) const
{
	const Int8ArrayTag& other2 = (const Int8ArrayTag&)(other);

	if (getId() != other2.getId())
		return false;

	if (other2.m_data != m_data)
		return false;

	return true;
}