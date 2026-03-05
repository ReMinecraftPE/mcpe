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

	void onPlace(TileSource*, const TilePos& pos) override;
	void neighborChanged(TileSource*, const TilePos& pos, TileID tile) override;
	void tick(TileSource*, const TilePos& pos, Random* random) override;
	void animateTick(TileSource*, const TilePos& pos, Random* random) override;
	AABB* getAABB(TileSource*, const TilePos& pos) override;
	float getBrightness(TileSource*, const TilePos& pos) const override;
	eRenderShape getRenderShape() const override;
	int getResource(TileData, Random*) const override;
	int getResourceCount(Random*) const override;
	int getTexture(Facing::Name face) const override;
	int getTexture(Facing::Name face, TileData data) const override;
	int getTickDelay() const override;
	void handleEntityInside(TileSource*, const TilePos& pos, const Entity*, Vec3&) override;
	bool isCubeShaped() const override;
	bool isSolidRender() const override;
	bool mayPick(TileData data, bool b) const override;
	bool shouldRenderFace(TileSource*, const TilePos& pos, Facing::Name face) const override;

	void updateLiquid(TileSource*, const TilePos& pos);
	void fizz(TileSource*, const TilePos& pos);
	int getColor(TileSource*, const TilePos& pos) const override;
	int getDepth(TileSource*, const TilePos& pos);
	int getRenderedDepth(TileSource*, const TilePos& pos) const;
	Vec3 getFlow(TileSource*, const TilePos& pos) const;

	static float getSlopeAngle(TileSource*, const TilePos& pos, const Material* pMtl);

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
