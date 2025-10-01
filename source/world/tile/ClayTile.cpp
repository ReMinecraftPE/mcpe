/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "ClayTile.hpp"
#include "world/level/Level.hpp"

ClayTile::ClayTile(TileID id, int texture, Material* c) : Tile(id, texture, c)
{
}

int ClayTile::getResource(TileData data, Random* random) const
{
	return 0; //@NOTE: Would be clay's item ID
}

int ClayTile::getResourceCount(Random* random) const
{
	return 4;
}
