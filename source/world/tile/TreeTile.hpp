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

	int getResource(int, Random*) override;
	int getResourceCount(Random*) override;
	int getSpawnResourcesAuxValue(int) override;
	int getTexture(int dir, int data) override;
	void onRemove(Level*, int x, int y, int z) override;
};
