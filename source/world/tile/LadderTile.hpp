/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "Tile.hpp"

class LadderTile : public Tile
{
public:
	LadderTile(int ID, int texture);

	bool isCubeShaped() override;
	bool isSolidRender() override;
	int getRenderShape() override;
	int getResourceCount(Random* random) override;
	AABB* getAABB(Level*, int x, int y, int z) override;
	AABB getTileAABB(Level*, int x, int y, int z) override;
	void setPlacedOnFace(Level*, int x, int y, int z, int face) override;
	void neighborChanged(Level*, int, int, int, int) override;
	bool mayPlace(Level*, int, int, int) override;
};
