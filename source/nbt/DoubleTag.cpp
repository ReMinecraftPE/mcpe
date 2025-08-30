#include <sstream>

#include "DoubleTag.hpp"

DoubleTag::DoubleTag() : m_data(0.0) {}
DoubleTag::DoubleTag(double val) : m_data(val) {}

void DoubleTag::write(IDataOutput& dos) const
{
	dos.writeDouble(m_data);
}

void DoubleTag::load(IDataInput& dis)
{
    m_data = dis.readDouble();
}

std::string DoubleTag::toString() const
{
	std::ostringstream sstream;
	sstream << m_data;
	return sstream.str();
}

Tag* DoubleTag::copy() const
{
	return new DoubleTag(m_data);
}

bool DoubleTag::operator==(const Tag& other) const
{
	const DoubleTag& other2 = (const DoubleTag&)(other);
	return getId() == other2.getId() && m_data == other2.m_data;
}