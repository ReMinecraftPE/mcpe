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

	void onPlace(Level*, const TilePos& pos) override;
	void tick(Level*, const TilePos& pos, Random*) override;

	bool checkSpongesNearby(Level*, const TilePos& pos);
	bool isWaterBlocking(Level*, const TilePos& pos);
	bool canSpreadTo(Level*, const TilePos& pos);
	int getSlopeDistance(Level*, const TilePos& pos, int depth, int);
	bool* getSpread(Level*, const TilePos& pos);
	void setStatic(Level*, const TilePos& pos);
	void trySpreadTo(Level*, const TilePos& pos, TileData data);
	int getSmallestDepth(Level*, const TilePos& pos, int oldDepth);
};
