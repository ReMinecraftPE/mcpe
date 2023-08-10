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

	void neighborChanged(Level* level, int x, int y, int z, int dir) override;
	void tick(Level* level, int x, int y, int z, Random* random) override;

	bool isFlammable(Level*, int x, int y, int z);
	void setDynamic(Level*, int x, int y, int z);
};
