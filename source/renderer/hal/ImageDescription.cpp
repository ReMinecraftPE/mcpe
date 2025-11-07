#include "ImageDescription.hpp"
#include "renderer/hal/helpers/TextureHelper.hpp"

using namespace mce;

ImageDescription::ImageDescription()
{
	width = 0;
	height = 0;
	pixels = nullptr;
	textureFormat = TEXTURE_FORMAT_UNKNOWN;
}

unsigned int ImageDescription::getSizeInBytes() const
{
	uint32_t size = width * height;
	return TextureHelper::textureFormatToByteStride(textureFormat) * size;
}
