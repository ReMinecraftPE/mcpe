/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "client/common/Utils.hpp"
#include "world/level/Material.hpp"

class LevelSource
{
public:
	virtual ~LevelSource();
	virtual TileID getTile(int x, int y, int z) = 0;
	virtual float getBrightness(int x, int y, int z) = 0;
	virtual int getData(int x, int y, int z) = 0;
	virtual Material* getMaterial(int x, int y, int z) = 0;
	virtual bool isSolidTile(int x, int y, int z) = 0;
};

