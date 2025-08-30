#include "Tag.hpp"
#include "EndTag.hpp"

EndTag::EndTag()
{
}

std::string EndTag::toString() const
{
	return "END";
}

Tag* EndTag::copy() const
{
	return new EndTag();
}

bool EndTag::operator==(const Tag& other) const
{
	return getId() == other.getId();
}