/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "LiquidTile.hpp"

class LiquidTileDynamic : public LiquidTile
{
public:
	LiquidTileDynamic(int id, Material* pMtl);

	void onPlace(TileSource*, const TilePos& pos) override;
	void tick(TileSource*, const TilePos& pos, Random*) override;

	bool checkSpongesNearby(TileSource*, const TilePos& pos);
	bool isWaterBlocking(TileSource*, const TilePos& pos);
	bool canSpreadTo(TileSource*, const TilePos& pos);
	int getSlopeDistance(TileSource*, const TilePos& pos, int depth, int);
	bool* getSpread(TileSource*, const TilePos& pos);
	void setStatic(TileSource*, const TilePos& pos);
	void trySpreadTo(TileSource*, const TilePos& pos, TileData data);
	int getSmallestDepth(TileSource*, const TilePos& pos, int oldDepth);
};
