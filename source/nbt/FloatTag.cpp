#include <sstream>

#include "FloatTag.hpp"

FloatTag::FloatTag() : m_data(0.0f) {}
FloatTag::FloatTag(float val) : m_data(val) {}

void FloatTag::write(IDataOutput& dos) const
{
    dos.writeFloat(m_data);
}

void FloatTag::load(IDataInput& dis)
{
    m_data = dis.readFloat();
}

std::string FloatTag::toString() const
{
	std::ostringstream sstream;
	sstream << m_data;
	return sstream.str();
}

Tag* FloatTag::copy() const
{
	return new FloatTag(m_data);
}

bool FloatTag::operator==(const Tag& other) const
{
	const FloatTag& other2 = (const FloatTag&)(other);
	return getId() == other2.getId() && m_data == other2.m_data;
}