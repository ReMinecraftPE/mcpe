#include "DeadBush.hpp"
#include "world/level/TileSource.hpp"

DeadBush::DeadBush(int id, int texture) : Bush(id, texture)
{
}

bool DeadBush::mayPlace(TileSource* source, const TilePos& pos) const
{
	TileID tile = source->getTile(pos.below());
	
	return tile == Tile::sand->m_ID;
}

int DeadBush::getResource(TileData data, Random* random) const
{
	return -1;
}
