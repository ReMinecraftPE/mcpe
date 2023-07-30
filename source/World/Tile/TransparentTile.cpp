/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "Tile.hpp"
#include "Level.hpp"

TransparentTile::TransparentTile(int ID, int texture, Material* mtl, bool bTransparent) :Tile(ID, texture, mtl)
{
	m_bTransparent = bTransparent;
}

bool TransparentTile::isSolidRender()
{
	return false;
}

bool TransparentTile::shouldRenderFace(LevelSource* level, int x, int y, int z, int dir)
{
	if (!m_bTransparent && level->getTile(x, y, z) == m_ID)
		return false;

	return Tile::shouldRenderFace(level, x, y, z, dir);
}
