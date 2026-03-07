#include <limits.h>

#include "TilePos.hpp"
#include "world/phys/Vec3.hpp"
#include "world/level/levelgen/chunk/ChunkPos.hpp"

const TilePos TilePos::ZERO = TilePos(0, 0, 0);
const TilePos TilePos::ONE = TilePos(1, 1, 1);
const TilePos TilePos::MIN = TilePos(INT_MIN, INT_MIN, INT_MIN);
const TilePos TilePos::MAX = TilePos(INT_MAX, INT_MAX, INT_MAX);

TilePos::TilePos(const ChunkPos& pos, int y)
{
    _init(pos.x * 16, y, pos.z * 16);
}
