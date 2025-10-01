/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "Tile.hpp"

class TreeTile : public Tile
{
public:
	TreeTile(int ID);

	int getResource(TileData, Random*) const override;
	int getResourceCount(Random*) const override;
	int getSpawnResourcesAuxValue(int) const override;
	int getTexture(Facing::Name face, TileData data) const override;
	void onRemove(Level*, const TilePos& pos) override;
};
