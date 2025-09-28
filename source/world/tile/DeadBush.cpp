#include "DeadBush.hpp"
#include "world/level/Level.hpp"

DeadBush::DeadBush(int id, int texture) : Bush(id, texture)
{
}

bool DeadBush::mayPlace(const Level* level, const TilePos& pos) const
{
	TileID tile = level->getTile(pos.below());
	
	return tile == Tile::sand->m_ID;
}

int DeadBush::getResource(int x, Random* random) const
{
	return -1;
}
