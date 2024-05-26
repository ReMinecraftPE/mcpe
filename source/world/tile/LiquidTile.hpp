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
	AABB* getAABB(const Level*, int x, int y, int z) override;
	float getBrightness(const LevelSource*, int x, int y, int z) const override;
	int getRenderLayer() const override;
	int getRenderShape() const override;
	int getResource(int, Random*) const override;
	int getResourceCount(Random*) const override;
	int getTexture(int dir) const override;
	int getTexture(int dir, int data) const override;
	int getTickDelay() const override;
	void handleEntityInside(Level*, int, int, int, const Entity*, Vec3&) override;
	bool isCubeShaped() const override;
	bool isSolidRender() const override;
	bool mayPick(int data, bool b) const override;
	bool shouldRenderFace(const LevelSource*, int x, int y, int z, int dir) const override;

	void updateLiquid(Level*, int x, int y, int z);
	void fizz(Level*, int x, int y, int z);
	int getColor(const LevelSource*, int x, int y, int z) const override;
	int getDepth(Level*, int x, int y, int z);
	int getRenderedDepth(const LevelSource*, int x, int y, int z) const;
	Vec3 getFlow(const LevelSource*, int x, int y, int z) const;

	static float getSlopeAngle(const LevelSource*, int x, int y, int z, const Material* pMtl);

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
