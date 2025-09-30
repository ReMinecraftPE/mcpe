#pragma once

#include <stdint.h>
#include "world/Facing.hpp"

#ifndef __VEC3_HPP
class Vec3;
#endif
#ifndef __CHUNKPOS_HPP
struct ChunkPos;
#endif

#define __TILEPOS_HPP

struct TilePos
{
public:
	static const TilePos ZERO, ONE, MIN, MAX;

public:
	int x;
	int y; // We had this on uint8_t due to the 255 height limit, but this can overflow too easily
	int z;
    
private:
    void _init(int _x, int _y, int _z);
    
public:
	TilePos();
	TilePos(int _x, int _y, int _z);
	TilePos(float _x, float _y, float _z);
	TilePos(const Vec3& pos);
	TilePos(const ChunkPos& pos, int y);

	TilePos relative(Facing::Name facing, int steps = 1) const;

	TilePos above(int steps = 1) const { return relative(Facing::UP, steps); }
	TilePos below(int steps = 1) const { return relative(Facing::DOWN, steps); }
	TilePos north(int steps = 1) const { return relative(Facing::NORTH, steps); }
	TilePos south(int steps = 1) const { return relative(Facing::SOUTH, steps); }
	TilePos west(int steps = 1) const  { return relative(Facing::WEST, steps); }
	TilePos east(int steps = 1) const  { return relative(Facing::EAST, steps); }

	bool operator<(const TilePos& b) const;
	bool operator>(const TilePos& b) const;
	bool operator<=(const TilePos& b) const;
	bool operator>=(const TilePos& b) const;
	TilePos operator+(const TilePos& b) const;
	TilePos operator+(int i) const;
	Vec3 operator+(float f) const;
	TilePos operator-(const TilePos& b) const;
	TilePos operator-(int i) const;
	Vec3 operator-(float f) const;
	void operator+=(const TilePos& b);
	void operator-=(const TilePos& b);
	void operator*=(int i);
	TilePos operator-() const;
	TilePos operator*(int i) const;
	Vec3 operator*(float f) const;
	TilePos operator/(int i) const;
	Vec3 operator/(float f) const;
	bool operator==(const TilePos& b) const;
	bool operator!=(const TilePos& b) const;
};