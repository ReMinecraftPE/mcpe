#include "CropsTile.hpp"
#include "world/level/Level.hpp"

CropsTile::CropsTile(int id, int texture) : Bush(id, texture)
{
	setTicking(true);
}

bool CropsTile::isSolidRender() const
{
	return false;
}

bool CropsTile::isCubeShaped() const
{
	return false;
}

eRenderShape CropsTile::getRenderShape() const
{	
	return SHAPE_CROPS;
}

void CropsTile::tick(Level* level, const TilePos& pos, Random* random)
{
	Bush::tick(level, pos, random);

    // Too dark
	if (level->getRawBrightness(pos.above()) < 9)
        return;

    int growthStage = level->getData(pos);
    // Fully grown
    if (growthStage >= 7)
        return;

    float growthRate = getGrowthRate(level, pos);
    if (random->nextInt((int)(100.0f / growthRate)) == 0)
    {
        level->setData(pos, growthStage + 1);
    }
}

float CropsTile::getGrowthRate(Level* level, const TilePos& pos)
{
	float rate = 1.0f;

	TileID north = level->getTile(pos.north());
	TileID south = level->getTile(pos.south());
	TileID west = level->getTile(pos.west());
	TileID east = level->getTile(pos.east());
	TileID nw = level->getTile(pos + TilePos(-1, 0, -1));
	TileID ne = level->getTile(pos + TilePos(1, 0, -1));
	TileID se = level->getTile(pos + TilePos(1, 0, 1));
	TileID sw = level->getTile(pos + TilePos(-1, 0, 1));
	
	bool hor = west == m_ID || east == m_ID;
	bool vert = north == m_ID || south == m_ID;
	bool diag = nw == m_ID || ne == m_ID || se == m_ID || sw == m_ID;

	TilePos tp = pos.below();
	for (tp.x = pos.x; tp.x <= pos.x + 1; ++tp.x)
	{
		for (tp.z = pos.z; tp.z <= pos.z + 1; ++tp.z)
		{
			TileID closeTile = level->getTile(tp);
			float rateInfluence = 0.0f;
			if (closeTile == Tile::farmland->m_ID)
			{
				rateInfluence = 1.0f;
				if (level->getData(tp) > 0)
				{
					rateInfluence = 3.0f;
				}
			}

			if (tp.x != pos.x || tp.z!= pos.z)
			{
				rateInfluence /= 4.0f;
			}

			rate += rateInfluence;
		}
	}

	if (diag || (hor && vert))
	{
		rate /= 2.0f;
	}

	return rate;
}

void CropsTile::growCropsToMax(Level* level, const TilePos& pos)
{
	level->setData(pos, 7);
}

int CropsTile::getResource(TileData data, Random* random) const
{
	return (data == 7) ? Item::wheat->m_itemID : -1;
}

void CropsTile::spawnResources(Level* level, const TilePos& pos, TileData data)
{
	Bush::spawnResources(level, pos, data);

	if (level->m_bIsClientSide)
		return;

	for (int i = 0; i < 3; i++)
	{
		if (level->m_random.nextInt(15) > data)
			continue;

		Vec3 deviation((level->m_random.nextFloat() * 0.7f) + (1.0f - 0.7f) * 0.5f,
			(level->m_random.nextFloat() * 0.7f) + (1.0f - 0.7f) * 0.5f,
			(level->m_random.nextFloat() * 0.7f) + (1.0f - 0.7f) * 0.5f);

		ItemStack item(Item::seeds, 1, getSpawnResourcesAuxValue(data));
		ItemEntity* pEntity = new ItemEntity(level, pos + deviation, item);
		pEntity->m_throwTime = 10;

		level->addEntity(pEntity);
	}
}

void CropsTile::neighborChanged(Level* level, const TilePos& pos, TileID tile)
{
	if (level->getTile(pos.below()) != Tile::farmland->m_ID)
	{
		level->setTile(pos, TILE_AIR);

		float spread = 0.7f;
		TilePos spreadPos(
            level->m_random.nextFloat() * spread + (1.0f - spread) * 0.5f,
            1.2f,
            level->m_random.nextFloat() * spread + (1.0f - spread) * 0.5f
        );

        ItemEntity* itemEntity = new ItemEntity(level, pos.above() + spreadPos, ItemStack(Item::seeds));
        itemEntity->m_throwTime = 10;

        level->addEntity(itemEntity);
	}
}


void CropsTile::updateShape(const LevelSource* level, const TilePos& pos)
{
	setShape(0, 0, 0, 1, 0.25f, 1);
}


int CropsTile::getTexture(Facing::Name face, TileData data) const
{
	if (data < 0)
		data = 7;

	return m_TextureFrame + data;

}
