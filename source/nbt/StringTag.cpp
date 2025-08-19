#include <sstream>

#include "StringTag.hpp"

StringTag::StringTag() {}
StringTag::StringTag(const std::string& data) : m_data(data) {}

void StringTag::write(IDataOutput& dos) const
{
	dos.writeString(m_data);
}

void StringTag::load(IDataInput& dis)
{
	m_data = dis.readString();
}

std::string StringTag::toString() const
{
	return m_data;
}

Tag* StringTag::copy() const
{
	return new StringTag(m_data);
}

bool StringTag::operator==(const Tag& other) const
{
	const StringTag& other2 = (const StringTag&)(other);
	return getId() == other2.getId() && m_data == other2.m_data;
}