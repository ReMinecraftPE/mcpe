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
	HitResult clip(const Level*, int x, int y, int z, Vec3, Vec3) override;
	AABB* getAABB(const Level*, int x, int y, int z) override;
	int getRenderShape() const override;
	int getResource(int data, Random*) const override;
	int getTexture(int dir, int data) const override;
	AABB getTileAABB(const Level*, int x, int y, int z) override;
	bool isCubeShaped() const override;
	bool isSolidRender() const override;
	bool mayPlace(const Level*, int x, int y, int z) const override;
	void updateShape(const LevelSource*, int x, int y, int z) override;
	void neighborChanged(Level*, int x, int y, int z, int newTile) override;

	bool blocksLight() const;
	int getDir(int data) const;
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
