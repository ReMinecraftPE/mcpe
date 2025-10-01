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

int TreeTile::getResource(TileData data, Random* random) const
{
	return Tile::treeTrunk->m_ID;
}

int TreeTile::getResourceCount(Random* random) const
{
	return 1;
}

int TreeTile::getSpawnResourcesAuxValue(int x) const
{
	return x;
}

int TreeTile::getTexture(Facing::Name face, TileData data) const
{
	if (face == Facing::UP || face == Facing::DOWN)
		return TEXTURE_LOG_TOP;

	switch (data)
	{
	case 1:
		return TEXTURE_LOG_SPRUCE;
	case 2:
		return TEXTURE_LOG_BIRCH;
	}

	return TEXTURE_LOG_SIDE;
}

void TreeTile::onRemove(Level* level, const TilePos& pos)
{
	// signal to nearby leaves that they should decay
	if (!level->hasChunksAt(pos - 5, pos + 5))
		return;

	TilePos tp;
	for (tp.x = -4; tp.x <= 5; tp.x++)
	{
		for (tp.y = -4; tp.y <= 5; tp.y++)
		{
			for (tp.z = -4; tp.z <= 5; tp.z++)
			{
				TileID tid = level->getTile(pos + tp);
				if (tid != Tile::leaves->m_ID) continue;

				TileData data = level->getData(pos + tp);
				if (data & 4)
					continue;

				level->setDataNoUpdate(pos + tp, data | 4);
			}
		}
	}
}
