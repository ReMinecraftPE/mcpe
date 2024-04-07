/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "Tile.hpp"

class LeverTile : public Tile
{
public:
	LeverTile(int ID, int texture, Material* pMtl);

	AABB* getAABB(Level*, int x, int y, int z) override;
	bool isSolidRender() override;
	bool isCubeShaped() override;
	int getRenderShape() override;
	HitResult clip(Level*, int x, int y, int z, Vec3 a, Vec3 b) override;
	bool mayPlace(Level*, int, int, int) override;
	void neighborChanged(Level*, int, int, int, int) override;
	void onPlace(Level*, int, int, int) override;
	void setPlacedOnFace(Level*, int, int, int, int) override;
	int use(Level* pLevel, int x, int y, int z, Player* player) override;
	void destroy(Level*, int, int, int, int dir) override;
	int getSignal(LevelSource*, int x, int y, int z, int dir) override;
	int getDirectSignal(LevelSource*, int x, int y, int z, int dir) override;
	bool isSignalSource() override;

	bool checkCanSurvive(Level*, int x, int y, int z);
};
