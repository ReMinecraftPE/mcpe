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
	Bush(int id, int texture);

	virtual bool canSurvive(const Level*, int x, int y, int z) const override;
	virtual AABB* getAABB(const Level*, int x, int y, int z) override;
	virtual int getRenderShape() const override;
	virtual bool isCubeShaped() const override;
	virtual bool isSolidRender() const override;
	virtual bool mayPlace(const Level*, int x, int y, int z) const override;
	virtual void tick(Level*, int x, int y, int z, Random*) override;
	virtual void neighborChanged(Level*, int x, int y, int z, int dir) override;

	void checkAlive(Level*, int x, int y, int z);
};
