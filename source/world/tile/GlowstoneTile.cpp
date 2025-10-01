#include "GlowstoneTile.hpp"
#include "world/level/Level.hpp"

GlowstoneTile::GlowstoneTile(TileID id, int texture, Material* material) : Tile(id, texture, material)
{
}

int GlowstoneTile::getResource(TileData data, Random*) const
{
	return Item::yellowDust->m_itemID;
}

int GlowstoneTile::getResourceCount(Random* random) const
{
	return 2 + random->nextInt(3);
}