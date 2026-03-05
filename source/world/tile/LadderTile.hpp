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

public:
	bool isCubeShaped() const override;
	bool isSolidRender() const override;
	eRenderShape getRenderShape() const override;
	int getResourceCount(Random* random) const override;
	AABB* getAABB(TileSource*, const TilePos& pos) override;
	AABB getTileAABB(TileSource*, const TilePos& pos) override;
	void setPlacedOnFace(TileSource*, const TilePos& pos, Facing::Name face) override;
	void neighborChanged(TileSource*, const TilePos& pos, TileID tile) override;
	bool mayPlace(TileSource*, const TilePos& pos) const override;
};
