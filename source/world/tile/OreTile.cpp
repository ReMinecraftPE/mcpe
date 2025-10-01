/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "OreTile.hpp"
#include "world/level/Level.hpp"

OreTile::OreTile(TileID id, int texture) : Tile(id, texture, Material::stone)
{

}

int OreTile::getResource(TileData data, Random* random) const
{
	return m_ID;
}

int OreTile::getResourceCount(Random* random) const
{
	if (m_ID == Tile::lapisOre->m_ID)
		return random->genrand_int32() % 5 + 4;

	return 1;
}

int OreTile::getSpawnResourcesAuxValue(int x) const
{
	return 0;
}
