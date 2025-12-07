#pragma once

#include <stdint.h>
#include "enums/TextureFormat.hpp"

namespace mce
{
	struct ImageDescription
	{
        unsigned int width;
        unsigned int height;
        void* pixels;
        TextureFormat textureFormat;
		
		ImageDescription();
		
		unsigned int getSizeInBytes() const;
	};
}
