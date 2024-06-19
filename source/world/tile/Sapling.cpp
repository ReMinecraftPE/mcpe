/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "Sapling.hpp"
#include "world/level/Level.hpp"

Sapling::Sapling(int id, int texture) : Bush(id, texture)
{
}

int Sapling::getTexture(Facing::Name face, int data) const
{
	return TEXTURE_SAPLING; // we don't have the other saplings' textures...
}

void Sapling::tick(Level* level, const TilePos& pos, Random* random)
{
	Bush::tick(level, pos, random);

	if (level->getRawBrightness(pos) > 8 && random->nextInt(7) == 0)
	{
		int data = level->getData(pos);

		if (data & 8)
			growTree(level, pos, random);
		else
			level->setDataNoUpdate(pos, data | 8);
	}
}

bool Sapling::maybeGrowTree(Level* level, const TilePos& pos, Random* random)
{
	// this is fine... these are not heavy at all
	TreeFeature treeFeature;
	BirchFeature birchFeature;
	SpruceFeature spruceFeature;

	Feature* pFeature = &treeFeature;

	int data = level->getData(pos);
	switch (data)
	{
		case 1:
			pFeature = &birchFeature;
			break;
		case 2:
			pFeature = &spruceFeature;
			break;
	}

	return treeFeature.place(level, random, pos);
}

void Sapling::growTree(Level* level, const TilePos& pos, Random* random)
{
	level->setTileNoUpdate(pos, TILE_AIR);

	if (!maybeGrowTree(level, pos, random))
		level->setTileNoUpdate(pos, m_ID);
}
