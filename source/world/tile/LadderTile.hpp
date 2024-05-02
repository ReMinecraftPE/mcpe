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

	bool isCubeShaped() const override;
	bool isSolidRender() const override;
	int getRenderShape() const override;
	int getResourceCount(Random* random) const override;
	AABB* getAABB(const Level*, int x, int y, int z) override;
	AABB getTileAABB(const Level*, int x, int y, int z) override;
	void setPlacedOnFace(Level*, int x, int y, int z, int face) override;
	void neighborChanged(Level*, int, int, int, int) override;
	bool mayPlace(const Level*, int, int, int) const override;
};
