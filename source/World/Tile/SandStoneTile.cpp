#include "Tile.hpp"
#include "Level.hpp"

SandStoneTile::SandStoneTile(int a, int b, Material* c) : Tile(a, b, c)
{
}

int SandStoneTile::getTexture(int side)
{
	if (side == DIR_YNEG)
		return TEXTURE_SANDSTONE_BOTTOM;

	if (side == DIR_YPOS)
		return TEXTURE_SANDSTONE_TOP;

	return m_TextureFrame;
}
