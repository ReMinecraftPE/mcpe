/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "Tile.hpp"

class DoorTile : public Tile
{
public:
	DoorTile(int ID, Material*);

	void attack(Level*, int x, int y, int z, Player*) override;
	int use(Level*, int x, int y, int z, Player*) override;
	HitResult clip(Level*, int x, int y, int z, Vec3, Vec3) override;
	AABB* getAABB(Level*, int x, int y, int z) override;
	int getRenderShape() override;
	int getResource(int data, Random*) override;
	int getTexture(int dir, int data) override;
	AABB getTileAABB(Level*, int x, int y, int z) override;
	bool isCubeShaped() override;
	bool isSolidRender() override;
	bool mayPlace(Level*, int x, int y, int z) override;
	void updateShape(LevelSource*, int x, int y, int z) override;
	void neighborChanged(Level*, int x, int y, int z, int newTile) override;

	bool blocksLight();
	int getDir(int data);
	void setOpen(Level*, int x, int y, int z, bool bOpen);
	
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Woverloaded-virtual"
	void setShape(int dir);
#pragma GCC diagnostic pop

	// @NOTE: These are inlined.
	inline static bool isOpen(int data)
	{
		return (data & 4) != 0;
	}
	inline static bool isTop(int data)
	{
		return (data & 8) != 0;
	}
};
