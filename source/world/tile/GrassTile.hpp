/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "Tile.hpp"

class GrassTile : public Tile
{
public:
	GrassTile(int ID, Material*);

	int getResource(int, Random*) override;
	int getColor(LevelSource*, int x, int y, int z) override;
	int getTexture(int dir) override;
	int getTexture(LevelSource*, int x, int y, int z, int dir) override;
	void tick(Level*, int x, int y, int z, Random*) override;
};
