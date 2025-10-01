/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "ObsidianTile.hpp"
#include "world/level/Level.hpp"

ObsidianTile::ObsidianTile(int a, int b, Material* c) : Tile(a, b, c)
{
}

int ObsidianTile::getResource(TileData data, Random* random) const
{
	return Tile::obsidian->m_ID;
}

int ObsidianTile::getResourceCount(Random* random) const
{
	return 1;
}
