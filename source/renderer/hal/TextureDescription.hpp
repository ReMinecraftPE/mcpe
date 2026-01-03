#pragma once

#include <stdint.h>
#include "ImageDescription.hpp"
#include "enums/TextureFiltering.hpp"

namespace mce
{
	struct TextureDescription : public ImageDescription
	{
        unsigned int sampleCount;
        unsigned int mipCount;
        bool bWrap;
        TextureFiltering filteringLevel;
        bool bIsStaging;
		
		TextureDescription();
	};
}
