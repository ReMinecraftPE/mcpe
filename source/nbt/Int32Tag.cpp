#include <sstream>

#include "Int32Tag.hpp"

Int32Tag::Int32Tag() : m_data(0) {}
Int32Tag::Int32Tag(int32_t val) : m_data(val) {}

void Int32Tag::write(IDataOutput& dos) const
{
    dos.writeInt32(m_data);
}

void Int32Tag::load(IDataInput& dis)
{
	m_data = dis.readInt32();
}

std::string Int32Tag::toString() const
{
	std::ostringstream sstream;
	sstream << m_data;
	return sstream.str();
}

Tag* Int32Tag::copy() const
{
	return new Int32Tag(m_data);
}

bool Int32Tag::operator==(const Tag& other) const
{
	const Int32Tag& other2 = (const Int32Tag&)(other);
	return getId() == other2.getId() && m_data == other2.m_data;
}