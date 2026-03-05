#include "world/level/levelgen/chunk/ChunkViewSource.hpp"

ChunkViewSource::ChunkViewSource(ChunkSource& parent, LoadMode parentLoadMode)
	: ChunkSource(parent),
	  m_parentLoadMode(parentLoadMode)
{
}

ChunkViewSource::~ChunkViewSource()
{
}

LevelChunk* ChunkViewSource::getExistingChunk(const ChunkPos& pos)
{
	Vec3Int32 gridPos(pos.x, 0, pos.z);
	if (m_gridArea.contains(gridPos))
		return m_gridArea.at(gridPos);

	return nullptr;
}

void ChunkViewSource::move(const TilePos& tileMin, const TilePos& tileMax)
{
	if (m_gridArea.getBounds().getSides() != 0)
	{
		m_gridArea.move(tileMin, tileMax);
	}
	else
	{
		m_gridArea = m_parent->getView(tileMin, tileMax, m_parentLoadMode, false, nullptr, nullptr);
	}
}

void ChunkViewSource::clear()
{
	m_gridArea.clear();
}
