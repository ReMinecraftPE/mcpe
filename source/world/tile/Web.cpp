#include "Web.hpp"
#include "world/level/Level.hpp"

Web::Web(int id, int texture) : Tile(id, texture, Material::web)
{
}

int Web::getRenderShape() const
{
	return SHAPE_CROSS;
}

bool Web::isCubeShaped() const
{
	return false;
}

bool Web::isSolidRender() const
{
	return false;
}

void Web::entityInside(Level*, const TilePos& pos, Entity* entity) const
{
	entity->m_bInWeb = true;
}

int Web::getResource(int x, Random* random) const
{
	return Item::string->m_itemID;
}

AABB* Web::getAABB(const Level* level, const TilePos& pos)
{
	return nullptr;
}
