/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "Tile.hpp"

class ReedTile : public Tile
{
public:
	ReedTile(int id);

	bool canSurvive(const Level*, int x, int y, int z) const override;
	AABB* getAABB(const Level*, int x, int y, int z) override;
	int getRenderShape() const override;
	bool isCubeShaped() const override;
	bool isSolidRender() const override;
	bool mayPlace(const Level*, int x, int y, int z) const override;
	void tick(Level*, int x, int y, int z, Random*) override;
	void neighborChanged(Level*, int x, int y, int z, int dir) override;
	int getResource(int, Random*) const override;

	void checkAlive(Level*, int x, int y, int z);
};
