#include <sstream>

#include "Int64Tag.hpp"

Int64Tag::Int64Tag() : m_data(0) {}
Int64Tag::Int64Tag(int64_t val) : m_data(val) {}

void Int64Tag::write(IDataOutput& dos) const
{
    dos.writeInt64(m_data);
}

void Int64Tag::load(IDataInput& dis)
{
    m_data = dis.readInt64();
}

std::string Int64Tag::toString() const
{
	std::ostringstream sstream;
	sstream << m_data;
	return sstream.str();
}

Tag* Int64Tag::copy() const
{
	return new Int64Tag(m_data);
}

bool Int64Tag::operator==(const Tag& other) const
{
	const Int64Tag& other2 = (const Int64Tag&)(other);
	return getId() == other2.getId() && m_data == other2.m_data;
}