/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "Tile.hpp"
#include "Level.hpp"

BookshelfTile::BookshelfTile(int a, int b, Material* c) : Tile(a, b, c)
{
}

int BookshelfTile::getTexture(int dir)
{
	if (dir <= DIR_YPOS)
		return TEXTURE_PLANKS;

	return m_TextureFrame;
}

int BookshelfTile::getResource(int data, Random* random)
{
	return 0; // would be Book
}

int BookshelfTile::getResourceCount(Random* random)
{
	return 3;
}


