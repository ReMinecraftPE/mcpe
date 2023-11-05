#include "GrassColor.hpp"

// TODO: This should be inside of an initialized "Minecraft" instance rather than the global namespace
bool g_bIsGrassColorAvailable = false;

Texture GrassColor::texture;

bool GrassColor::isAvailable()
{
	return g_bIsGrassColorAvailable;
}

void GrassColor::init(Texture texture)
{
	GrassColor::texture = texture;
}

uint32_t GrassColor::get(double x, double y)
{
	y *= x;
	return GrassColor::texture.m_pixels[(int)((1.0 - y) * 255.0) << 8 | (int)((1.0 - x) * 255.0)];
}