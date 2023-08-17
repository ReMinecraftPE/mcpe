/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "Tile.hpp"

class FireTile : public Tile
{
public:
	FireTile(int ID, int texture);

	AABB* getAABB(Level*, int x, int y, int z) override;
	int getRenderShape() override;
	bool isCubeShaped() override;
	bool isSolidRender() override;
	int getResourceCount(Random*) override;
	int getTickDelay() override;
	bool mayPick() override;
	bool mayPlace(Level*, int x, int y, int z) override;
	void animateTick(Level*, int x, int y, int z, Random*) override;
	void neighborChanged(Level*, int x, int y, int z, int dir) override;
	void onPlace(Level*, int x, int y, int z) override;
	void tick(Level*, int x, int y, int z, Random*) override;

	void checkBurn(Level*, int x, int y, int z, int, Random*);
	int getFireOdds(Level*, int x, int y, int z);
	bool isValidFireLocation(Level*, int x, int y, int z);

	// @NOTE: This is inlined in V0.1.0 but not V0.7.1
	inline bool canBurn(LevelSource* level, int x, int y, int z)
	{
		return m_igniteOdds[level->getTile(x, y, z)] > 0;
	}

public:
	int m_igniteOdds[C_MAX_TILES];
	int m_burnOdds[C_MAX_TILES];
};
