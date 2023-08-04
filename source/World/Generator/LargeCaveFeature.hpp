#pragma once

#include "LargeFeature.hpp"

class LargeCaveFeature : public LargeFeature
{
public:
	void addFeature(Level*, int ax, int az, int x, int z, TileID* tiles, int unk) override;
	
	void addRoom(int x, int z, TileID* tiles, float rx, float ry, float rz);
	void addTunnel(int x, int z, TileID* tiles, float rx, float ry, float rz, float x1, float ang, float x2, int x3, int x4, float x5);
};

