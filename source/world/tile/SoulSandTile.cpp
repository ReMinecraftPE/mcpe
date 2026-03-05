#include "SoulSandTile.hpp"
#include "world/level/TileSource.hpp"

SoulSandTile::SoulSandTile(int id, int texture) : Tile(id, texture, Material::sand)
{
}

void SoulSandTile::entityInside(TileSource* source, const TilePos& pos, Entity* entity) const
{
	entity->m_vel.x *= 0.4;
	entity->m_vel.z *= 0.4;
}

AABB* SoulSandTile::getAABB(TileSource* source, const TilePos& pos)
{
	AABB* rAABB = Tile::getAABB(source, pos);
	rAABB->max.y -= 2 / 16.0;
	return rAABB;
}
