#pragma once
#include <cmath>

#include "world/phys/Vec3.hpp"
#include "world/level/TilePos.hpp"

#define __CHUNKPOS_HPP

struct ChunkPos
{
	int x, z;
	
private:
	void _init(int _x, int _z) {
		x = _x;
		z = _z;
	}
	void _init(const Vec3& pos) {
		_init(TilePos(pos));
	}
	void _init(const TilePos& pos) {
		_init(ToChunkCoordinate(pos.x), ToChunkCoordinate(pos.z));
	}
	
public:
	ChunkPos() {
		_init(0, 0);
	}

	ChunkPos(int _x, int _z) {
		_init(_x, _z);
	}

	ChunkPos(int _x, int _y, int _z) {
		_init(TilePos(_x, _y, _z));
	}

	ChunkPos(float _x, float _y, float _z) {
		_init(Vec3(_x, _y, _z));
	}

	ChunkPos(const Vec3& pos) { _init(pos); }
	ChunkPos(const TilePos& pos) { _init(pos); }

	int lengthSqr() const {
		return x * x + z * z;
	}

	bool operator<(const ChunkPos& b) const {
		if (x != b.x)
			return x < b.x;

		return z < b.z;
	}

	bool operator>(const ChunkPos& b) const {
		if (x != b.x)
			return x > b.x;

		return z > b.z;
	}

	bool operator<=(const ChunkPos& b) const {
		return *this < b || *this == b;
	}

	bool operator>=(const ChunkPos& b) const {
		return *this > b || *this == b;
	}

	ChunkPos operator+(const ChunkPos& b) const {
		return ChunkPos(x + b.x, z + b.z);
	}

	ChunkPos operator+(int i) const {
		return ChunkPos(x + i, z + i);
	}

	ChunkPos operator-(const ChunkPos& b) const {
		return ChunkPos(x - b.x, z - b.z);
	}

	ChunkPos operator-(int i) const {
		return *this - ChunkPos(i, i);
	}

	void operator+=(const ChunkPos& b) {
		x += b.x;
		z += b.z;
	}

	void operator-=(const ChunkPos& b) {
		(*this) += -b;
	}

	void operator*=(int i) {
		x *= i;
		z *= i;
	}

	ChunkPos operator-() const {
		return ChunkPos(-x, -z);
	}

	ChunkPos operator*(int i) const {
		return ChunkPos(x * i, z * i);
	}

	ChunkPos operator/(int i) const {
		return ChunkPos(x / i, z / i);
	}

	bool operator==(const ChunkPos& other) const {
		return x == other.x &&
			z == other.z;
	}

	bool operator!=(const ChunkPos& other) const {
		return !(*this == other);
	}

	operator TilePos() const {
		return TilePos(*this, 0);
	}

	static int ToChunkCoordinate(int value)
	{
		return value >> 4;
	}

	static int ToChunkCoordinate(float value)
	{
		return int(floorf(value / 16));
	}
};
