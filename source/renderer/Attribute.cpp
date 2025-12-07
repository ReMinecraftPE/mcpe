#include "Attribute.hpp"

using namespace mce;

Attribute::Attribute()
{
	m_vertexField = VertexField();
	m_location = 0;
	m_count = 0;
}

Attribute::Attribute(unsigned int location, unsigned int count, VertexField vertexField)
{
	m_vertexField = vertexField;
	m_location = location;
	m_count = count;
}

bool Attribute::operator==(const Attribute &other) const
{
  return m_vertexField == other.m_vertexField &&
		 m_location == other.m_location &&
		 m_count == other.m_count;
}