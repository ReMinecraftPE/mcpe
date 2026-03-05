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

	AABB* getAABB(TileSource*, const TilePos& pos) override;
	eRenderShape getRenderShape() const override;
	bool isCubeShaped() const override;
	bool isSolidRender() const override;
	int getResourceCount(Random*) const override;
	int getTickDelay() const override;
	bool mayPick() const override;
	bool mayPlace(TileSource*, const TilePos& pos) const override;
	void animateTick(TileSource*, const TilePos& pos, Random*) override;
	void neighborChanged(TileSource*, const TilePos& pos, TileID tile) override;
	void onPlace(TileSource*, const TilePos& pos) override;
	void tick(TileSource*, const TilePos& pos, Random*) override;

	void checkBurn(TileSource*, const TilePos& pos, int, Random*);
	int getFireOdds(TileSource*, const TilePos& pos);
	bool isValidFireLocation(TileSource*, const TilePos& pos) const;

	bool canBurn(TileSource* source, const TilePos& pos) const;

public:
	int m_igniteOdds[C_MAX_TILES];
	int m_burnOdds[C_MAX_TILES];
};
