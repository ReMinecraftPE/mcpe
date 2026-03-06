#include "world/level/levelgen/chunk/Bounds.hpp"
#include "world/level/levelgen/chunk/ChunkPos.hpp"

static Vec3Int32 convertTilePos(const TilePos& pos, int sides, bool noSides)
{
	return Vec3Int32(
		ChunkPos::ToChunkCoordinate(pos.x),
		noSides ? 0 : (Mth::clamp(pos.y, 0, 127) / sides),
		ChunkPos::ToChunkCoordinate(pos.z)
	);
}

Bounds::Bounds(const TilePos& min, const TilePos& max, int sides, bool noSides)
	: _min(convertTilePos(min, sides, noSides)),
	  _max(convertTilePos(max, sides, noSides)),
	  _sides(sides)
{
	_dimensions = _max - _min + 1;
	_area = _dimensions.x * _dimensions.z;
	_volume = _dimensions.x * _dimensions.y * _dimensions.z;
}
