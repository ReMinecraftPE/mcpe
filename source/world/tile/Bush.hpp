/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "Tile.hpp"

class Bush : public Tile
{
public:
	Bush(TileID id, int texture);

	bool canSurvive(const Level*, const TilePos& pos) const override;
	AABB* getAABB(const Level*, const TilePos& pos) override;
	eRenderShape getRenderShape() const override;
	bool isCubeShaped() const override;
	bool isSolidRender() const override;
	bool mayPlace(const Level*, const TilePos& pos) const override;
	void tick(Level*, const TilePos& pos, Random*) override;
	void neighborChanged(Level*, const TilePos& pos, TileID tile) override;

	void checkAlive(Level*, const TilePos& pos);
};
