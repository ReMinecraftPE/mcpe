#pragma once

#include "renderer/hal/enums/VertexField.hpp"

namespace mce
{
	class Attribute
	{
	private:
		VertexField m_vertexField;
		unsigned int m_location;
		unsigned int m_count;
		
	public:
		Attribute();
		Attribute(unsigned int location, unsigned int count, VertexField vertexField);

		VertexField getVertexField() const { return m_vertexField; }
		unsigned int getLocation() const { return m_location; }
		
		bool operator==(const Attribute& other) const;
	};
}
