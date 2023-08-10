/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "Tile.hpp"

class TorchTile : public Tile
{
public:
	TorchTile(int ID, int texture, Material* pMtl);

	virtual AABB* getAABB(Level*, int x, int y, int z) override;
	virtual bool isSolidRender() override;
	virtual bool isCubeShaped() override;
	virtual int getRenderShape() override;
	virtual void animateTick(Level*, int x, int y, int z, Random*) override;
	virtual HitResult clip(Level*, int x, int y, int z, Vec3 a, Vec3 b) override;
	virtual bool mayPlace(Level*, int, int, int) override;
	virtual void neighborChanged(Level*, int, int, int, int) override;
	virtual void onPlace(Level*, int, int, int) override;
	virtual void setPlacedOnFace(Level*, int, int, int, int) override;
	virtual void tick(Level*, int, int, int, Random*) override;

	bool checkCanSurvive(Level*, int x, int y, int z);
};
