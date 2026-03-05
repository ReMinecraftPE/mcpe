/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "LiquidTile.hpp"

class LiquidTileStatic : public LiquidTile
{
public:
	LiquidTileStatic(int id, Material* pMtl);

	void neighborChanged(TileSource* source, const TilePos& pos, TileID tile) override;
	void tick(TileSource* source, const TilePos& pos, Random* random) override;

	bool isFlammable(TileSource*, const TilePos& pos);
	void setDynamic(TileSource*, const TilePos& pos);
};
