/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "Tile.hpp"

class TopSnowTile : public Tile
{
public:
	TopSnowTile(TileID id, int texture, Material* pMtl);

public:
	AABB* getAABB(TileSource*, const TilePos& pos) override;
	bool isCubeShaped() const override;
	bool isSolidRender() const override;
	int getResource(TileData data, Random*) const override;
	int getResourceCount(Random*) const override;
	bool mayPlace(TileSource*, const TilePos& pos) const override;
	void neighborChanged(TileSource*, const TilePos& pos, TileID tile) override;
	bool shouldRenderFace(TileSource*, const TilePos& pos, Facing::Name face) const override;
	void tick(TileSource*, const TilePos& pos, Random*) override;

	bool checkCanSurvive(TileSource*, const TilePos& pos);

	static FullTile dataIDToRecoverableFullTile(TileSource* source, const TilePos& pos, TileData tileDataID);
};
