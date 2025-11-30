#include "ChunkPos.hpp"
#include "world/level/TilePos.hpp"
#include "world/phys/Vec3.hpp"

void ChunkPos::_init(int _x, int _z)
{
    x = _x;
    z = _z;
}

void ChunkPos::_init(const Vec3& pos)
{
    _init(TilePos(pos));
}

void ChunkPos::_init(const TilePos& pos)
{
    _init(ToChunkCoordinate(pos.x), ToChunkCoordinate(pos.z));
}

ChunkPos::ChunkPos()
{
    _init(0, 0);
}

ChunkPos::ChunkPos(int _x, int _z)
{
    _init(_x, _z);
}

ChunkPos::ChunkPos(int _x, int _y, int _z)
{
    _init(TilePos(_x, _y, _z));
}

ChunkPos::ChunkPos(float _x, float _y, float _z)
{
    _init(Vec3(_x, _y, _z));
}

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