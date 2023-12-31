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
	RocketLauncherTile(int id, int texture);

	virtual AABB* getAABB(Level*, int x, int y, int z) override;
	virtual int getRenderShape() override;
	virtual bool isCubeShaped() override;
	virtual bool isSolidRender() override;
	virtual int use(Level* pLevel, int x, int y, int z, Player* player);
};
