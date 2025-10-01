/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "Tile.hpp"

class FarmTile : public Tile
{
public:
	FarmTile(TileID ID, Material*);

	AABB* getAABB(const Level*, const TilePos& pos) override;
	int getResource(TileData, Random*) const override;
	int getTexture(Facing::Name face, TileData data) const override;
	bool isCubeShaped() const override;
	bool isSolidRender() const override;
	void neighborChanged(Level*, const TilePos& pos, TileID tile) override;
	void stepOn(Level* level, const TilePos& pos, Entity* pEnt) override;
	void tick(Level* level, const TilePos& pos, Random* random) override;

	bool isNearWater(Level* level, const TilePos& pos);
};
