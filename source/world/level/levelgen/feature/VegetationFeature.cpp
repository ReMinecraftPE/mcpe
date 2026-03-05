#include "Feature.hpp"
#include "world/level/TileSource.hpp"

VegetationFeature::VegetationFeature(TileID id, TileData data, int count)
{
	m_ID = id;
	m_data = data;
	m_count = count;
}

bool VegetationFeature::place(TileSource* source, Random* random, const TilePos& pos)
{
	TilePos bPos(pos);
	TilePos tp;
	while (true)
	{
		TileID currentTile = source->getTile(bPos);
		if ((currentTile != TILE_AIR && currentTile != Tile::leaves->m_ID) || bPos.y <= 0)
		{
			for (int var7 = 0; var7 < m_count; ++var7)
			{
				tp.x = bPos.x + random->nextInt(8) - random->nextInt(8);
				tp.y = bPos.y + random->nextInt(4) - random->nextInt(4);
				tp.z = bPos.z + random->nextInt(8) - random->nextInt(8);
				if (source->isEmptyTile(tp) && Tile::tiles[m_ID]->canSurvive(source, tp))
				{
					source->setTileAndDataNoUpdate(tp, FullTile(m_ID, m_data));
				}
			}

			return true;
		}
		--bPos.y;
	}
}
