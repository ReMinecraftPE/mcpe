#pragma once

#include <stdint.h>
#include "world/level/TilePos.hpp"

struct ChunkTilePos
{
	uint8_t x, z, y; // in this order as of 0.9.2

	ChunkTilePos() : ChunkTilePos(0, 0, 0) {}
	ChunkTilePos(uint8_t _x, uint8_t _y, uint8_t _z) : x(_x), y(_y), z(_z) {}
	ChunkTilePos(const TilePos& pos) : ChunkTilePos(pos.x & 0xF, pos.y, pos.z & 0xF) {} // & 0xF on x and y to get them to uint8_t

	ChunkTilePos operator+(const TilePos& other) const
	{
		return ChunkTilePos(x + other.x, y + other.y, z + other.z);
	}

	operator TilePos() const { return TilePos(*this); }
};