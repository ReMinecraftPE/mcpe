/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "Tile.hpp"

class BookshelfTile : public Tile
{
public:
	BookshelfTile(int ID, int texture, Material*);

	int getTexture(Facing::Name face) const override;
	int getResource(TileData data, Random* random) const override;
	int getResourceCount(Random* random) const override;
};
