/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "Tile.hpp"
#include "Level.hpp"

HalfTransparentTile::HalfTransparentTile(int a, int b, Material* c) : Tile(a, b, c)
{
}

bool HalfTransparentTile::isSolidRender()
{
	return false;
}

bool HalfTransparentTile::shouldRenderFace(LevelSource* level, int x, int y, int z, int dir)
{
	if (field_6C || level->getTile(x, y, z) != m_ID)
		return Tile::shouldRenderFace(level, x, y, z, dir);
	
	return field_6C;
}