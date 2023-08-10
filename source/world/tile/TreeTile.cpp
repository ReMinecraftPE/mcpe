/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "TreeTile.hpp"
#include "world/level/Level.hpp"

TreeTile::TreeTile(int id) : Tile(id, Material::wood)
{
	m_TextureFrame = TEXTURE_LOG_SIDE;
}

int TreeTile::getResource(int x, Random* random)
{
	return Tile::treeTrunk->m_ID;
}

int TreeTile::getResourceCount(Random* random)
{
	return 1;
}

int TreeTile::getSpawnResourcesAuxValue(int x)
{
	return x;
}

int TreeTile::getTexture(int dir, int data)
{
	if (dir == DIR_YPOS || dir == DIR_YNEG)
		return TEXTURE_LOG_TOP;

	if (data == 1)
		return TEXTURE_LOG_SPRUCE;

	if (data == 2)
		return TEXTURE_LOG_BIRCH;

	return TEXTURE_LOG_SIDE;
}

void TreeTile::onRemove(Level* level, int x, int y, int z)
{
	// signal to nearby leaves that they should decay
	if (!level->hasChunksAt(x - 5, y - 5, z - 5, x + 5, y + 5, z + 5))
		return;

	for (int i = -4; i <= 5; i++)
	{
		for (int j = -4; j <= 5; j++)
		{
			for (int k = -4; k <= 5; k++)
			{
				TileID tid = level->getTile(x + i, y + j, z + k);
				if (tid != Tile::leaves->m_ID) continue;

				int data = level->getData(x + i, y + j, z + k);
				if (data & 4)
					continue;

				level->setDataNoUpdate(x + i, y + j, z + k, data | 4);
			}
		}
	}
}
