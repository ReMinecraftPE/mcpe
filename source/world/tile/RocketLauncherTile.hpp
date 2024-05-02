/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "Tile.hpp"

class RocketLauncherTile : public Tile
{
public:
	RocketLauncherTile(int id);

	int getTexture(int dir, int data) const override;
	AABB* getAABB(const Level*, int x, int y, int z) override;
	int getRenderShape() const override;
	bool isCubeShaped() const override;
	bool isSolidRender() const override;
	int use(Level* pLevel, int x, int y, int z, Player* player) override;
	void tick(Level*, int, int, int, Random*) override;
};
