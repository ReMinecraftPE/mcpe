#pragma once
#include "world/level/levelgen/chunk/ChunkSource.hpp"

class ChunkViewSource : public ChunkSource
{
protected:
	LoadMode m_parentLoadMode;
	GridArea<LevelChunk*> m_gridArea;

public:
	ChunkViewSource(ChunkSource& parent, LoadMode parentLoadMode);
	virtual ~ChunkViewSource();

public:
	LevelChunk* getExistingChunk(const ChunkPos& pos) override;
	void move(const TilePos& tileMin, const TilePos& tileMax);
	void clear();

	const Bounds& getBounds() const
	{
		return m_gridArea.getBounds();
	}
};
