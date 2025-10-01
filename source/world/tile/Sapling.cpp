/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "Sapling.hpp"
#include "world/level/Level.hpp"

Sapling::Sapling(TileID id, int texture) : Bush(id, texture)
{
}

int Sapling::getTexture(Facing::Name face, TileData data) const
{
	data &= 3;
	return data == 1 ? 63 : (data == 2 ? 79 : Bush::getTexture(face, data));
}

void Sapling::tick(Level* level, const TilePos& pos, Random* random)
{
	Bush::tick(level, pos, random);

	if (level->getRawBrightness(pos) > 8 && random->nextInt(7) == 0)
	{
		TileData data = level->getData(pos);

		if (data & 8)
			growTree(level, pos, random);
		else
			level->setDataNoUpdate(pos, data | 8);
	}
}

void Sapling::growTree(Level* level, const TilePos& pos, Random* random)
{
	TileData data = level->getData(pos) & 3;
	level->setTileNoUpdate(pos, TILE_AIR);

	TreeFeature treeFeature;

	Feature* pFeature = &treeFeature;

	switch (data)
	{
	case 1:
		pFeature = new SpruceFeature;
		break;
	case 2:
		pFeature = new BirchFeature;
		break;
	}

	if (!pFeature->place(level, random, pos))
		level->setTileNoUpdate(pos, m_ID);
}

int Sapling::getSpawnResourcesAuxValue(int x) const
{
	return x & 3;
}
