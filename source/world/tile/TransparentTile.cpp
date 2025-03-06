/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "TransparentTile.hpp"
#include "world/level/Level.hpp"

TransparentTile::TransparentTile(int ID, int texture, Material* mtl, bool bTransparent) :Tile(ID, texture, mtl)
{
	m_bTransparent = bTransparent;
}

bool TransparentTile::isSolidRender() const
{
	return false;
}

bool TransparentTile::shouldRenderFace(const LevelSource* level, const TilePos& pos, Facing::Name face) const
{
	if (!m_bTransparent && level->getTile(pos) == m_ID)
		return false;

	return Tile::shouldRenderFace(level, pos, face);
}
