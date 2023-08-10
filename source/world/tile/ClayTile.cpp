/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "ClayTile.hpp"
#include "world/level/Level.hpp"

ClayTile::ClayTile(int a, int b, Material* c) : Tile(a, b, c)
{
}

int ClayTile::getResource(int, Random* random)
{
	return 0; //@NOTE: Would be clay's item ID
}

int ClayTile::getResourceCount(Random* random)
{
	return 4;
}
