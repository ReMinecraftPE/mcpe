/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "world/level/Material.hpp"
#include "world/level/levelgen/biome/BiomeSource.hpp"

class LevelSource
{
public:
	virtual ~LevelSource();
	virtual TileID getTile(const TilePos& pos) const = 0;
	virtual float getBrightness(const TilePos& pos) const = 0;
	virtual TileData getData(const TilePos& pos) const = 0;
	virtual Material* getMaterial(const TilePos& pos) const = 0;
	virtual bool isSolidTile(const TilePos& pos) const = 0;
	virtual BiomeSource* getBiomeSource() const = 0;
};

