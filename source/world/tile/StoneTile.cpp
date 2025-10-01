/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "StoneTile.hpp"
#include "world/level/Level.hpp"

StoneTile::StoneTile(TileID id, int b, Material* c) : Tile(id, b, c)
{
}

int StoneTile::getResource(TileData data, Random* b) const
{
	return Tile::stoneBrick->m_ID;
}
