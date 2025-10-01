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

	virtual bool canSurvive(const Level*, const TilePos& pos) const override;
	virtual AABB* getAABB(const Level*, const TilePos& pos) override;
	virtual int getRenderShape() const override;
	virtual bool isCubeShaped() const override;
	virtual bool isSolidRender() const override;
	virtual bool mayPlace(const Level*, const TilePos& pos) const override;
	virtual void tick(Level*, const TilePos& pos, Random*) override;
	virtual void neighborChanged(Level*, const TilePos& pos, TileID tile) override;

	void checkAlive(Level*, const TilePos& pos);
};
