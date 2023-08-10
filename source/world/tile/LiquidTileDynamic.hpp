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

	void onPlace(Level*, int x, int y, int z) override;
	void tick(Level*, int x, int y, int z, Random*) override;

	bool checkSpongesNearby(Level*, int x, int y, int z);
	bool isWaterBlocking(Level*, int x, int y, int z);
	bool canSpreadTo(Level*, int x, int y, int z);
	int getSlopeDistance(Level*, int, int, int, int, int);
	bool* getSpread(Level*, int x, int y, int z);
	void setStatic(Level*, int x, int y, int z);
	void trySpreadTo(Level*, int x, int y, int z, int a);
	int getSmallestDepth(Level*, int x, int y, int z, int oldDepth);
};
