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

	AABB* getAABB(const Level*, const TilePos& pos) override;
	bool isSolidRender() const override;
	bool isCubeShaped() const override;
	eRenderShape getRenderShape() const override;
	void animateTick(Level*, const TilePos& pos, Random*) override;
	HitResult clip(const Level*, const TilePos& pos, Vec3 a, Vec3 b) override;
	bool mayPlace(const Level*, const TilePos& pos) const override;
	void neighborChanged(Level*, const TilePos& pos, TileID tile) override;
	void onPlace(Level*, const TilePos& pos) override;
	void setPlacedOnFace(Level*, const TilePos& pos, Facing::Name face) override;
	void tick(Level*, const TilePos& pos, Random*) override;

	bool checkCanSurvive(Level*, const TilePos& pos);
};
