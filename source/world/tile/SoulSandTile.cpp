#include "SoulSandTile.hpp"
#include "world/level/Level.hpp"

SoulSandTile::SoulSandTile(int id, int texture) : Tile(id, texture, Material::sand)
{
}

void SoulSandTile::entityInside(Level* level, const TilePos& pos, Entity* entity) const
{
	entity->m_vel.x *= 0.4;
	entity->m_vel.z *= 0.4;
}

AABB* SoulSandTile::getAABB(const Level* pLevel, const TilePos& pos)
{
	AABB* rAABB = Tile::getAABB(pLevel, pos);
	rAABB->max.y -= 2 / 16.0;
	return rAABB;
}
