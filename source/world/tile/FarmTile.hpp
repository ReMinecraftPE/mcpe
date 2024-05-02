/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "Tile.hpp"

class FarmTile : public Tile
{
public:
	FarmTile(int ID, Material*);

	AABB* getAABB(const Level*, int x, int y, int z) override;
	int getResource(int, Random*) const override;
	int getTexture(int dir, int data) const override;
	bool isCubeShaped() const override;
	bool isSolidRender() const override;
	void neighborChanged(Level*, int x, int y, int z, int dir) override;
	void stepOn(Level* level, int x, int y, int z, Entity* pEnt) override;
	void tick(Level* level, int x, int y, int z, Random* random) override;

	bool isNearWater(Level* level, int x, int y, int z);
};
