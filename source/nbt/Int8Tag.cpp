#include <sstream>

#include "Int8Tag.hpp"

Int8Tag::Int8Tag() : m_data(0) {}
Int8Tag::Int8Tag(int8_t val) : m_data(val) {}

void Int8Tag::write(IDataOutput& dos) const
{
    dos.writeInt8(m_data);
}

void Int8Tag::load(IDataInput& dis)
{
    m_data = dis.readInt8();
}

std::string Int8Tag::toString() const
{
	std::ostringstream sstream;
	sstream << m_data;
	return sstream.str();
}

Tag* Int8Tag::copy() const
{
	return new Int8Tag(m_data);
}

bool Int8Tag::operator==(const Tag& other) const
{
	const Int8Tag& other2 = (const Int8Tag&)(other);
	return getId() == other2.getId() && m_data == other2.m_data;
}