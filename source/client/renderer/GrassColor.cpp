#include "GrassColor.hpp"

bool GrassColor::_isAvailable = false;

Texture GrassColor::texture;

void GrassColor::init(Texture texture)
{
	GrassColor::texture = texture;
}

uint32_t GrassColor::get(double x, double y)
{
	y *= x;
	return GrassColor::texture.m_pixels[(int)((1.0 - y) * 255.0) << 8 | (int)((1.0 - x) * 255.0)];
}