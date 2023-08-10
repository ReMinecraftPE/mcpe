/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "Tile.hpp"

class TopSnowTile : public Tile
{
public:
	TopSnowTile(int id, int texture, Material* pMtl);

	AABB* getAABB(Level*, int x, int y, int z) override;
	bool isCubeShaped() override;
	bool isSolidRender() override;
	int getResource(int, Random*) override;
	int getResourceCount(Random*) override;
	bool mayPlace(Level*, int x, int y, int z) override;
	void neighborChanged(Level*, int x, int y, int z, int d) override;
	bool shouldRenderFace(LevelSource*, int x, int y, int z, int i) override;
	void tick(Level*, int x, int y, int z, Random*) override;

	bool checkCanSurvive(Level*, int x, int y, int z);
};
