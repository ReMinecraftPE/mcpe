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
	: m_min(convertTilePos(min, sides, noSides)),
	  m_max(convertTilePos(max, sides, noSides)),
	  m_sides(sides)
{
	m_dimensions = m_max - m_min + 1;
	m_area = m_dimensions.x * m_dimensions.z;
	m_volume = m_dimensions.x * m_dimensions.y * m_dimensions.z;
}
