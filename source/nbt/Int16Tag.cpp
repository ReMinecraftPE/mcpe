#include <sstream>

#include "Int16Tag.hpp"

Int16Tag::Int16Tag() : m_data(0) {}
Int16Tag::Int16Tag(int16_t val) : m_data(val) {}

void Int16Tag::write(IDataOutput& dos) const
{
    dos.writeInt16(m_data);
}

void Int16Tag::load(IDataInput& dis)
{
	m_data = dis.readInt16();
}

std::string Int16Tag::toString() const
{
	std::ostringstream sstream;
	sstream << m_data;
	return sstream.str();
}

Tag* Int16Tag::copy() const
{
	return new Int16Tag(m_data);
}

bool Int16Tag::operator==(const Tag& other) const
{
	const Int16Tag& other2 = (const Int16Tag&)(other);
	return getId() == other2.getId() && m_data == other2.m_data;
}