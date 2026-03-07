#pragma once

#include <stdint.h>
#include "world/Facing.hpp"
#include "world/phys/Vec3.hpp"
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
    void _init(int _x, int _y, int _z) {
		x = _x;
		y = _y;
		z = _z;
	}
    
public:
	TilePos() {
		_init(0, 0, 0);
	}
	TilePos(int _x, int _y, int _z) {
		_init(_x, _y, _z);
	}
	TilePos(float _x, float _y, float _z) {
		_init((int)floorf(_x), (int)floorf(_y), (int)floorf(_z));
	}
	TilePos(const Vec3& pos) {
		//@NOTE: Using floor fixes TilePos instantiation in negative coords
		_init((int)floorf(pos.x), (int)floorf(pos.y), (int)floorf(pos.z));
	}
	TilePos(const ChunkPos& pos, int y);

	TilePos relative(Facing::Name facing, int steps = 1) const {
		switch (facing) {
			case Facing::NORTH: return TilePos(x, y, z - steps);
			case Facing::SOUTH: return TilePos(x, y, z + steps);
			case Facing::UP:    return TilePos(x, y + steps, z);
			case Facing::DOWN:  return TilePos(x, y - steps, z);
			case Facing::WEST:  return TilePos(x - steps, y, z);
			case Facing::EAST:  return TilePos(x + steps, y, z);
			default:            return TilePos(*this);
		}
	}

	TilePos above(int steps = 1) const { return relative(Facing::UP, steps); }
	TilePos below(int steps = 1) const { return relative(Facing::DOWN, steps); }
	TilePos north(int steps = 1) const { return relative(Facing::NORTH, steps); }
	TilePos south(int steps = 1) const { return relative(Facing::SOUTH, steps); }
	TilePos west(int steps = 1) const  { return relative(Facing::WEST, steps); }
	TilePos east(int steps = 1) const  { return relative(Facing::EAST, steps); }

	bool operator<(const TilePos& b) const {
		if (x != b.x)
			return x < b.x;
		if (y != b.y)
			return y < b.y;

		return z < b.z;
	}

	bool operator>(const TilePos& b) const {
		if (x != b.x)
			return x > b.x;
		if (y != b.y)
			return y > b.y;

		return z > b.z;
	}

	bool operator<=(const TilePos& b) const {
		return x <= b.x &&
			y <= b.y &&
			z <= b.z;
	}

	bool operator>=(const TilePos& b) const {
		return x >= b.x &&
			y >= b.y &&
			z >= b.z;
	}

	TilePos operator+(const TilePos& b) const {
		return TilePos(x + b.x, y + b.y, z + b.z);
	}

	TilePos operator+(int i) const {
		return TilePos(i + x, i + y, i + z);
	}

	Vec3 operator+(float f) const {
		return Vec3(*this) + f;
	}

	TilePos operator-(const TilePos& b) const {
		return TilePos(x - b.x, y - b.y, z - b.z);
	}

	TilePos operator-(int i) const {
		return TilePos(x - i, y - i, z - i);
	}

	Vec3 operator-(float f) const {
		return Vec3(*this) - f;
	}

	void operator+=(const TilePos& b) {
		x += b.x;
		y += b.y;
		z += b.z;
	}

	void operator-=(const TilePos& b) {
		(*this) += -b;
	}

	void operator*=(int i) {
		x *= i;
		y *= i;
		z *= i;
	}

	TilePos operator-() const {
		return TilePos(-x, -y, -z);
	}

	TilePos operator*(int i) const {
		return TilePos(x * i, y * i, z * i);
	}

	Vec3 operator*(float f) const {
		return Vec3(*this) * f;
	}

	TilePos operator/(int i) const {
		return TilePos(x / i, y / i, z / i);
	}

	Vec3 operator/(float f) const {
		return Vec3(*this) / f;
	}

	bool operator==(const TilePos& b) const {
		return x == b.x && y == b.y && z == b.z;
	}

	bool operator!=(const TilePos& b) const {
		// Dunno if the top is more optimal than the bottom when compiled
		//return x != b.x || z != b.z;
		return !(*this == b);
	}

	operator Vec3() const {
		return Vec3(x, y, z);
	}
};
