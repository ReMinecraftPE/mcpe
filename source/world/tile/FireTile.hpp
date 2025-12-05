/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "Tile.hpp"

class FireTile : public Tile
{
public:
	FireTile(int ID, int texture);

	AABB* getAABB(const Level*, const TilePos& pos) override;
	eRenderShape getRenderShape() const override;
	bool isCubeShaped() const override;
	bool isSolidRender() const override;
	int getResourceCount(Random*) const override;
	int getTickDelay() const override;
	bool mayPick() const override;
	bool mayPlace(const Level*, const TilePos& pos) const override;
	void animateTick(Level*, const TilePos& pos, Random*) override;
	void neighborChanged(Level*, const TilePos& pos, TileID tile) override;
	void onPlace(Level*, const TilePos& pos) override;
	void tick(Level*, const TilePos& pos, Random*) override;

	void checkBurn(Level*, const TilePos& pos, int, Random*);
	int getFireOdds(const Level*, const TilePos& pos);
	bool isValidFireLocation(const Level*, const TilePos& pos) const;

	// @NOTE: This is inlined in V0.1.0 but not V0.7.1
	inline bool canBurn(const LevelSource* level, const TilePos& pos) const
	{
		return m_igniteOdds[level->getTile(pos)] > 0;
	}

public:
	int m_igniteOdds[C_MAX_TILES];
	int m_burnOdds[C_MAX_TILES];
};
