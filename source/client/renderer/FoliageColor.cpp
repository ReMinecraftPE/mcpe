#include "FoliageColor.hpp"

bool FoliageColor::_isAvailable = false;

Texture FoliageColor::texture;

void FoliageColor::init(Texture texture)
{
	FoliageColor::texture = texture;
}

uint32_t FoliageColor::get(double x, double y)
{
	y *= x;
	return FoliageColor::texture.m_pixels[(int)((1.0 - y) * 255.0) << 8 | (int)((1.0 - x) * 255.0)];
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