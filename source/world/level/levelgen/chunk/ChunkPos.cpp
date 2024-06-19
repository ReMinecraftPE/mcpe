#include "ChunkPos.hpp"
#include "world/level/TilePos.hpp"
#include "world/phys/Vec3.hpp"

ChunkPos::ChunkPos()
	: ChunkPos(0, 0) {}

ChunkPos::ChunkPos(int _x, int _z)
	: x(_x), z(_z) {}

ChunkPos::ChunkPos(int _x, int _y, int _z)
	: ChunkPos(TilePos(_x, _y, _z)) {}

ChunkPos::ChunkPos(float _x, float _y, float _z)
	: ChunkPos(Vec3(_x, _y, _z)) {}

ChunkPos::ChunkPos(const Vec3& pos)
	: ChunkPos(TilePos(pos)) {}

ChunkPos::ChunkPos(const TilePos& pos)
	: ChunkPos(pos.x >> 4, pos.z >> 4) {}

bool ChunkPos::operator<(const ChunkPos& b) const
{
	if (x != b.x)
		return x < b.x;

	return z < b.z;
}

bool ChunkPos::operator>(const ChunkPos& b) const
{
	if (x != b.x)
		return x > b.x;

	return z > b.z;
}

bool ChunkPos::operator<=(const ChunkPos& b) const
{
	return *this < b || *this == b;
}

bool ChunkPos::operator>=(const ChunkPos& b) const
{
	return *this > b || *this == b;
}

ChunkPos ChunkPos::operator+(const ChunkPos& b) const
{
	return ChunkPos(x + b.x, z + b.z);
}

ChunkPos ChunkPos::operator+(int i) const
{
	return ChunkPos(x +i , z + i);
}

ChunkPos ChunkPos::operator-(const ChunkPos& b) const
{
	return ChunkPos(x - b.x, z - b.z);
}

ChunkPos ChunkPos::operator-(int i) const
{
	return *this - ChunkPos(i, i);
}

void ChunkPos::operator+=(const ChunkPos& b)
{
	x += b.x;
	z += b.z;
}

void ChunkPos::operator-=(const ChunkPos& b)
{
	(*this) += -b;
}

void ChunkPos::operator*=(int i)
{
	x *= i;
	z *= i;
}

ChunkPos ChunkPos::operator-() const
{
	return ChunkPos(-x, -z);
}

ChunkPos ChunkPos::operator*(int i) const
{
	return ChunkPos(x * i, z * i);
}

ChunkPos ChunkPos::operator/(int i) const
{
	return ChunkPos(x / i, z / i);
}

bool ChunkPos::operator==(const ChunkPos& other) const
{
	return x == other.x &&
		z == other.z;
}

bool ChunkPos::operator!=(const ChunkPos& other) const
{
	return !(*this == other);
}

ChunkPos::operator TilePos() const
{
	return TilePos(*this, 0);
}