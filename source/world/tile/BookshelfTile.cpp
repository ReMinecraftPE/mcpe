/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "BookshelfTile.hpp"
#include "world/level/Level.hpp"

BookshelfTile::BookshelfTile(int a, int b, Material* c) : Tile(a, b, c)
{
}

int BookshelfTile::getTexture(Facing::Name face) const
{
	if (face <= Facing::UP)
		return TEXTURE_PLANKS;

	return m_TextureFrame;
}

int BookshelfTile::getResource(TileData data, Random* random) const
{
	return 0; // would be Book
}

int BookshelfTile::getResourceCount(Random* random) const
{
	return 3;
}


