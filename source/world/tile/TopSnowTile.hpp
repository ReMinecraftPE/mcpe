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

	AABB* getAABB(const Level*, int x, int y, int z) override;
	bool isCubeShaped() const override;
	bool isSolidRender() const override;
	int getResource(int, Random*) const override;
	int getResourceCount(Random*) const override;
	bool mayPlace(const Level*, int x, int y, int z) const override;
	void neighborChanged(Level*, int x, int y, int z, int d) override;
	bool shouldRenderFace(const LevelSource*, int x, int y, int z, int i) const override;
	void tick(Level*, int x, int y, int z, Random*) override;

	bool checkCanSurvive(Level*, int x, int y, int z);
};
