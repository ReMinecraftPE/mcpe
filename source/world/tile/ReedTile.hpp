/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "Tile.hpp"

class ReedTile : public Tile
{
public:
	ReedTile(TileID id);

	bool canSurvive(TileSource*, const TilePos& pos) const override;
	AABB* getAABB(TileSource*, const TilePos& pos) override;
	eRenderShape getRenderShape() const override;
	bool isCubeShaped() const override;
	bool isSolidRender() const override;
	bool mayPlace(TileSource*, const TilePos& pos) const override;
	void tick(TileSource*, const TilePos& pos, Random*) override;
	void neighborChanged(TileSource*, const TilePos& pos, TileID tile) override;
	int getResource(TileData data, Random*) const override;

	void checkAlive(TileSource*, const TilePos& pos);
};
