#include "TilePos.hpp"
#include "world/phys/Vec3.hpp"
#include "world/level/levelgen/chunk/ChunkPos.hpp"

TilePos::TilePos()
	: TilePos(0, 0, 0) {}

TilePos::TilePos(int _x, int _y, int _z)
	: x(_x), y(_y < 0 ? 0 : _y), z(_z) {}

TilePos::TilePos(float _x, float _y, float _z)
	: TilePos((int)floorf(_x), (uint8_t)floorf(_y), (int)floorf(_z)) {}

TilePos::TilePos(const Vec3& pos)
	: TilePos(pos.x, pos.y, pos.z) {}

TilePos::TilePos(const ChunkPos& pos, int y)
	: TilePos(pos.x * 16, y, pos.z * 16) {}

TilePos TilePos::relative(Facing::Name facing, int steps) const
{
	switch (facing)
	{
	case Facing::NORTH: return TilePos(x,         y,         z - steps);
	case Facing::SOUTH: return TilePos(x,         y,         z + steps);
	case Facing::UP:    return TilePos(x,         y + steps, z);
	case Facing::DOWN:  return TilePos(x,         y - steps, z);
	case Facing::WEST:  return TilePos(x - steps, y,         z);
	case Facing::EAST:  return TilePos(x + steps, y,         z);
	default:            return TilePos(*this);
	}
}

bool TilePos::operator<(const TilePos& b) const
{
	if (x != b.x)
		return x < b.x;
	if (y != b.y)
		return y < b.y;

	return z < b.z;
}

bool TilePos::operator>(const TilePos& b) const
{
	if (x != b.x)
		return x > b.x;
	if (y != b.y)
		return y > b.y;

	return z > b.z;
}

bool TilePos::operator<=(const TilePos& b) const
{
	return x <= b.x &&
		   y <= b.y &&
		   z <= b.z;
}

bool TilePos::operator>=(const TilePos& b) const
{
	return x >= b.x &&
		   y >= b.y &&
		   z >= b.z;
}

TilePos TilePos::operator+(const TilePos& b) const
{
	return TilePos(x + b.x, y + b.y, z + b.z);
}

TilePos TilePos::operator+(int i) const
{
	return TilePos(i + x, i + y, i + z);
}

Vec3 TilePos::operator+(float f) const
{
	return Vec3(*this) + f;
}

TilePos TilePos::operator-(const TilePos& b) const
{
	return TilePos(x - b.x, y - b.y, z - b.z);
}

TilePos TilePos::operator-(int i) const
{
	return TilePos(x - i, y - i, z - i);
}

Vec3 TilePos::operator-(float f) const
{
	return Vec3(*this) - f;
}

void TilePos::operator+=(const TilePos& b)
{
	x += b.x;
	y += b.y;
	z += b.z;
}

void TilePos::operator-=(const TilePos& b)
{
	(*this) += -b;
}

void TilePos::operator*=(int i)
{
	x *= i;
	y *= i;
	z *= i;
}

TilePos TilePos::operator-() const
{
	return TilePos(-x, -y, -z);
}

TilePos TilePos::operator*(int i) const
{
	return TilePos(x * i, y * i, z * i);
}

Vec3 TilePos::operator*(float f) const
{
	return Vec3(*this) * f;
}

TilePos TilePos::operator/(int i) const
{
	return TilePos(x / i, y / i, z / i);
}

Vec3 TilePos::operator/(float f) const
{
	return Vec3(*this) / f;
}

bool TilePos::operator==(const TilePos& b) const
{
	return x == b.x && y == b.y && z == b.z;
}

bool TilePos::operator!=(const TilePos& b) const
{
	// Dunno if the top is more optimal than the bottom when compiled
	//return x != b.x || z != b.z;
	return !(*this == b);
}