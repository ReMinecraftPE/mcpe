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

public:
	AABB* getAABB(TileSource*, const TilePos& pos) override;
	bool isSolidRender() const override;
	bool isCubeShaped() const override;
	eRenderShape getRenderShape() const override;
	void animateTick(TileSource*, const TilePos& pos, Random*) override;
	HitResult clip(TileSource*, const TilePos& pos, Vec3 a, Vec3 b) override;
	bool mayPlace(TileSource*, const TilePos& pos) const override;
	void neighborChanged(TileSource*, const TilePos& pos, TileID tile) override;
	void onPlace(TileSource*, const TilePos& pos) override;
	void setPlacedOnFace(TileSource*, const TilePos& pos, Facing::Name face) override;
	void tick(TileSource*, const TilePos& pos, Random*) override;

	bool checkCanSurvive(TileSource*, const TilePos& pos);
};
