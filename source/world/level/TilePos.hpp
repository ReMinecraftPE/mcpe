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

	int volume() const
	{
		return x * y * z;
	}

	TilePos min(const TilePos& other) const
	{
		return TilePos(
			x < other.x ? x : other.x,
			y < other.y ? y : other.y,
			z < other.z ? z : other.z
		);
	}

	TilePos max(const TilePos& other) const
	{
		return TilePos(
			x < other.x ? other.x : x,
			y < other.y ? other.y : y,
			z < other.z ? other.z : z
		);
	}

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
	TilePos operator*(const TilePos& other) const
	{
		return TilePos(x * other.x, y * other.y, z * other.z);
	}
	Vec3 operator*(float f) const;
	TilePos operator/(int i) const;
	Vec3 operator/(float f) const;
	bool operator==(const TilePos& b) const;
	bool operator!=(const TilePos& b) const;

	int& operator[](size_t i)
	{
		return reinterpret_cast<int*>(this)[i];
	}

	const int& operator[](size_t i) const
	{
		return reinterpret_cast<const int*>(this)[i];
	}

	operator int*()
	{
		return reinterpret_cast<int*>(this);
	}

	operator const int*() const
	{
		return reinterpret_cast<const int*>(this);
	}
};
