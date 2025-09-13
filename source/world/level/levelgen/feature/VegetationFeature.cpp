#include "Feature.hpp"
#include "world/level/Level.hpp"

VegetationFeature::VegetationFeature(int id, int data, int count)
{
	m_ID = id;
	m_data = data;
	m_count = count;
}

bool VegetationFeature::place(Level* level, Random* random, const TilePos& pos)
{
	TilePos bPos(pos);
	TilePos tp;
	while (true)
	{
		int var11 = level->getTile(bPos);
		if (var11 != 0 && var11 != Tile::leaves->m_ID || bPos.y <= 0)
		{
			for (int var7 = 0; var7 < m_count; ++var7)
			{
				tp.x = bPos.x + random->nextInt(8) - random->nextInt(8);
				tp.y = bPos.y + random->nextInt(4) - random->nextInt(4);
				tp.z = bPos.z + random->nextInt(8) - random->nextInt(8);
				if (level->isEmptyTile(tp) && Tile::tiles[m_ID]->canSurvive(level, tp))
				{
					if (m_data) level->setTileAndDataNoUpdate(tp, m_ID, m_data);
					else level->setTileNoUpdate(tp, m_ID);
				}
			}

			return true;
		}
		--bPos.y;
	}
}
