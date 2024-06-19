/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "Tile.hpp"

class RocketLauncherTile : public Tile
{
public:
	RocketLauncherTile(int id);

	int getTexture(Facing::Name face, int data) const override;
	AABB* getAABB(const Level*, const TilePos& pos) override;
	int getRenderShape() const override;
	bool isCubeShaped() const override;
	bool isSolidRender() const override;
	int use(Level* pLevel, const TilePos& pos, Player* player) override;
	void tick(Level*, const TilePos& pos, Random*) override;
};
