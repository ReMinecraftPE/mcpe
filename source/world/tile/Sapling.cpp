/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "Sapling.hpp"
#include "world/level/TileSource.hpp"
#include "world/level/levelgen/feature/Feature.hpp"

Sapling::Sapling(TileID id, int texture) : Bush(id, texture)
{
}

int Sapling::getTexture(Facing::Name face, TileData data) const
{
	data &= 3;
	return data == 1 ? 63 : (data == 2 ? 79 : Bush::getTexture(face, data));
}

void Sapling::tick(TileSource* source, const TilePos& pos, Random* random)
{
	Bush::tick(source, pos, random);

	if (source->getRawBrightness(pos, true) > 8 && random->nextInt(7) == 0)
	{
		TileData data = source->getData(pos);

		if (data & 8)
			growTree(source, pos, random);
		else
			source->setTileAndDataNoUpdate(pos, FullTile(m_ID, data | 8));
	}
}

void Sapling::growTree(TileSource* source, const TilePos& pos, Random* random)
{
	TileData data = source->getData(pos) & 3;
	source->setTileNoUpdate(pos, TILE_AIR);

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

	if (!pFeature->place(source, random, pos))
		source->setTileNoUpdate(pos, m_ID);
}

int Sapling::getSpawnResourcesAuxValue(int x) const
{
	return x & 3;
}
