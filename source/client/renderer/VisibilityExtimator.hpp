#pragma once

#include <deque>

#include "common/Stopwatch.hpp"
#include "common/ByteMask.hpp"
#include "world/level/TilePos.hpp"
#include "world/level/levelgen/chunk/ChunkTilePos.hpp"
#include "world/tile/Tile.hpp"
#include "client/renderer/RenderChunk.hpp"

class VisibilityNode
{
protected:
	ByteMask m_visibility[Facing::MAX];

public:
	VisibilityNode(bool empty);
	void setEmpty();
	void setOpaque();
	void connect(const ByteMask& group);
	void connect(uint8_t A, const ByteMask& connected);
	void connect(uint8_t A, uint8_t B);
	const ByteMask& from(uint8_t facing) const
	{
		return m_visibility[facing];
	}
	bool compare(VisibilityNode& other) const;
};

class VisibilityExtimator
{
protected:
	static const int TILES_SIZE = 4096;
	static const int TILE_COUNT_EMPTY_THRESHOLD = 3840;

protected:
	enum TileState
	{
		TS_EMPTY,
		TS_OPAQUE,
		TS_EMPTY_MARKED
	};

public:
	// TODO
	//static ThreadLocal<VisibilityExtimator> pool;

public:
#ifdef _DEBUG
	Stopwatch m_timer;
#endif
protected:
	TilePos m_origin;
	int m_emptyTiles;
	uint8_t m_tiles[TILES_SIZE];
	std::deque<ChunkTilePos> m_floodQueue;

public:
	VisibilityExtimator()
		: m_origin(),
		  m_emptyTiles(0),
		  m_tiles()
	{
	}
public:
	void start(const RenderChunk& parent);
	void setTile(const TilePos& pos, const Tile* t);
	bool isAllOpaque() const
	{
		return m_emptyTiles == 0;
	}
	bool isAllEmpty() const
	{
		return m_emptyTiles >= TILE_COUNT_EMPTY_THRESHOLD;
	}
	VisibilityNode finish();
protected:
	uint8_t* _at(const ChunkTilePos& pos, ByteMask& set);
	uint8_t& _at(const ChunkTilePos& p);
	uint8_t& _atWorld(const TilePos& t);
	void _visit(const ChunkTilePos& p, ByteMask& set);
	ByteMask _floodFill(const ChunkTilePos& startPos);
};
