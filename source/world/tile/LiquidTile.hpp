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

	virtual void onPlace(Level*, const TilePos& pos) override;
	virtual void neighborChanged(Level*, const TilePos& pos, TileID tile) override;
	virtual void tick(Level*, const TilePos& pos, Random* random) override;
	void animateTick(Level*, const TilePos& pos, Random* random) override;
	AABB* getAABB(const Level*, const TilePos& pos) override;
	float getBrightness(const LevelSource*, const TilePos& pos) const override;
	int getRenderLayer() const override;
	int getRenderShape() const override;
	int getResource(TileData, Random*) const override;
	int getResourceCount(Random*) const override;
	int getTexture(Facing::Name face) const override;
	int getTexture(Facing::Name face, TileData data) const override;
	int getTickDelay() const override;
	void handleEntityInside(Level*, const TilePos& pos, const Entity*, Vec3&) override;
	bool isCubeShaped() const override;
	bool isSolidRender() const override;
	bool mayPick(TileData data, bool b) const override;
	bool shouldRenderFace(const LevelSource*, const TilePos& pos, Facing::Name face) const override;

	void updateLiquid(Level*, const TilePos& pos);
	void fizz(Level*, const TilePos& pos);
	int getColor(const LevelSource*, const TilePos& pos) const override;
	int getDepth(Level*, const TilePos& pos);
	int getRenderedDepth(const LevelSource*, const TilePos& pos) const;
	Vec3 getFlow(const LevelSource*, const TilePos& pos) const;

	static float getSlopeAngle(const LevelSource*, const TilePos& pos, const Material* pMtl);

	// @NOTE: This is inlined in minecraftcpp
	static float getWaterVolume(TileData data)
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
