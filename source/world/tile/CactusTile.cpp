#include "CactusTile.hpp"
#include "world/level/TileSource.hpp"

CactusTile::CactusTile(int id, int texture) : Tile(id, texture, Material::cactus)
{
	m_renderLayer = RENDER_LAYER_ALPHATEST;
	setTicking(true);
}

AABB* CactusTile::getAABB(TileSource* source, const TilePos& pos)
{
	AABB* aabb = Tile::getAABB(source, pos);
	aabb->max.y -= 1.0f / 16.0f;
	return aabb;
}

bool CactusTile::mayPlace(TileSource* source, const TilePos& pos) const
{
	return Tile::mayPlace(source, pos) && canSurvive(source, pos);
}

bool CactusTile::canSurvive(TileSource* source, const TilePos& pos) const
{
	for (int i = Facing::NORTH; i <= Facing::EAST; ++i)
	{
		if (source->getMaterial(pos.relative((Facing::Name)i))->isSolid())
		{
			return false;
		}
	}

	TileID tile = source->getTile(pos.below());

	return tile == Tile::sand->m_ID || tile == m_ID;
}


void CactusTile::neighborChanged(TileSource* source, const TilePos& pos, TileID tile)
{
	if (!canSurvive(source, pos))
	{
		spawnResources(source, pos, source->getData(pos));
		source->setTile(pos, TILE_AIR);
	}
}

bool CactusTile::isSolidRender() const
{
	return false;
}

bool CactusTile::isCubeShaped() const
{
	return false;
}

eRenderShape CactusTile::getRenderShape() const
{
	return SHAPE_CACTUS;
}

void CactusTile::tick(TileSource* source, const TilePos& pos, Random* random)
{
	TilePos above = pos.above();
	if (source->isEmptyTile(pos.above()))
	{
		int height;
		for (height = 1; source->getTile(pos.below(height)) == m_ID; ++height)
		{
		}
		if (height < 3) 
		{
			TileData data = source->getData(pos);
			if (data == 15) 
			{
				source->setTileAndData(above, FullTile(m_ID, 0));
			}
			else 
			{
				source->setTileAndData(pos, FullTile(m_ID, data + 1));
			}
		}
	}
}

void CactusTile::updateShape(TileSource* source, const TilePos& pos)
{
	setShape(0.0625, 0, 0.0625, 0.9375, 1, 0.9375);
}

void CactusTile::entityInside(TileSource* source, const TilePos& pos, Entity* entity) const
{
	entity->hurt(nullptr, 1);
}

int CactusTile::getTexture(Facing::Name face) const
{
	return face == 1 ? m_TextureFrame - 1 : (face == 0 ? m_TextureFrame + 1 : m_TextureFrame);
}
