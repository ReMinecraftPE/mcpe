#include "FoliageColor.hpp"
#include "compat/EndianDefinitions.h"

bool FoliageColor::_isAvailable = false;

Texture FoliageColor::texture;

void FoliageColor::init(Texture texture)
{
	FoliageColor::texture = texture;
}

uint32_t FoliageColor::get(double x, double y)
{
	y *= x;
	uint32_t c = FoliageColor::texture.m_pixels[(int)((1.0 - y) * 255.0) << 8 | (int)((1.0 - x) * 255.0)];
	
	// @TODO: same as in GrassColor::get, should be abstracted
#if MC_ENDIANNESS_BIG
	uint8_t r = c & 0xFF, g = (c >> 8) & 0xFF, b = (c >> 16) & 0xFF, a = (c >> 24) & 0xFF;
	c = a | (b << 8) | (g << 16) | (r << 24);
#endif
	
	return c;
}

uint32_t FoliageColor::getEvergreenColor()
{
	return 0x619961;
}

uint32_t FoliageColor::getBirchColor()
{
	return 0x55A780;
}

uint32_t FoliageColor::getDefaultColor()
{
	return 0x18B548;
}