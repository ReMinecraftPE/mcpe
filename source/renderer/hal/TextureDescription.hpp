#pragma once

#include <stdint.h>
#include "ImageDescription.hpp"
#include "enums/TextureFiltering.hpp"

namespace mce
{
	struct TextureDescription : public ImageDescription
	{
        unsigned int unknown1;
        unsigned int mipCount;
        bool bWrap;
        TextureFiltering filteringLevel;
        uint8_t unknown3;
		
		TextureDescription();
	};
}
