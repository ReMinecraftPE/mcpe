#include "GrassColor.hpp"
#include "compat/EndianDefinitions.h"

bool GrassColor::_isAvailable = false;

Texture GrassColor::texture;

void GrassColor::init(Texture texture)
{
	GrassColor::texture = texture;
}

uint32_t GrassColor::get(double x, double y)
{
	y *= x;
	uint32_t c = GrassColor::texture.m_pixels[(int)((1.0 - y) * 255.0) << 8 | (int)((1.0 - x) * 255.0)];
	
#if MC_ENDIANNESS_BIG
	uint8_t r = c & 0xFF, g = (c >> 8) & 0xFF, b = (c >> 16) & 0xFF, a = (c >> 24) & 0xFF;
	c = a | (b << 8) | (g << 16) | (r << 24);
#endif
	
	return c;
}