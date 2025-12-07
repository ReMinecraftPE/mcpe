#include "StencilRefObject.hpp"

using namespace mce;

StencilRefObject::StencilRefObject(uint8_t ref)
{
    stencilRef = ref;
}

bool StencilRefObject::operator==(const StencilRefObject& other) const
{
	return stencilRef == other.stencilRef;
}

bool StencilRefObject::operator!=(const StencilRefObject& other) const
{
	return !(*this == other);
}