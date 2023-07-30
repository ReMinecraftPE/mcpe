#pragma once

#include "Utils.hpp"
#include "Material.hpp"

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

