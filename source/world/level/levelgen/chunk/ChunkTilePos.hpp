#pragma once

#include <stdint.h>
#include "world/level/TilePos.hpp"
#include "world/level/levelgen/chunk/ChunkConstants.hpp"

struct ChunkTilePos
{
	uint8_t x, z, y; // in this order as of 0.9.2
    
private:
	void _init(uint8_t _x, uint8_t _y, uint8_t _z) { x = _x; y = _y; z = _z; }
    
public:
	ChunkTilePos() { _init(0, 0, 0); }
	ChunkTilePos(uint8_t _x, uint8_t _y, uint8_t _z) { _init(_x, _y, _z); }
	ChunkTilePos(const TilePos& pos) { _init(pos.x & (ChunkConstants::XZ_SIZE - 1), pos.y, pos.z & (ChunkConstants::XZ_SIZE - 1)); }

	ChunkTilePos operator+(const ChunkTilePos& other) const
	{
		return ChunkTilePos(x + other.x, y + other.y, z + other.z);
	}

	TilePos operator+(const TilePos& other) const
	{
		return TilePos(x + other.x, y + other.y, z + other.z);
	}

	int index() const
	{
		return (x << 11) | (z << 7) | y;
	}

	int index2D() const
	{
		return (z << 4) | x;
	}
};
