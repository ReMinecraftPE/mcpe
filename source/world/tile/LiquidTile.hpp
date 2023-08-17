/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "Tile.hpp"

class LiquidTile : public Tile
{
public:
	LiquidTile(int ID, Material*);

	virtual void onPlace(Level*, int x, int y, int z) override;
	virtual void neighborChanged(Level*, int x, int y, int z, int dir) override;
	virtual void tick(Level*, int x, int y, int z, Random* random) override;
	void animateTick(Level*, int x, int y, int z, Random* random) override;
	AABB* getAABB(Level*, int x, int y, int z) override;
	float getBrightness(LevelSource*, int x, int y, int z) override;
	int getRenderLayer() override;
	int getRenderShape() override;
	int getResource(int, Random*) override;
	int getResourceCount(Random*) override;
	int getTexture(int dir) override;
	int getTexture(int dir, int data) override;
	int getTickDelay() override;
	void handleEntityInside(Level*, int, int, int, Entity*, Vec3&) override;
	bool isCubeShaped() override;
	bool isSolidRender() override;
	bool mayPick(int data, bool b) override;
	bool shouldRenderFace(LevelSource*, int x, int y, int z, int dir) override;

	void updateLiquid(Level*, int x, int y, int z);
	void fizz(Level*, int x, int y, int z);
	int getColor(LevelSource*, int x, int y, int z) override;
	int getDepth(Level*, int x, int y, int z);
	int getRenderedDepth(LevelSource*, int x, int y, int z);
	Vec3 getFlow(LevelSource*, int x, int y, int z);

	static float getSlopeAngle(LevelSource*, int x, int y, int z, Material* pMtl);

	// @NOTE: This is inlined in minecraftcpp
	static float getWaterVolume(int data)
	{
		if (data >= 8)
			data = 0;

		return float(data + 1) / 9.0f;
	}

public:
	int field_6C;
	bool field_70[4];
	int field_74[4];
};
