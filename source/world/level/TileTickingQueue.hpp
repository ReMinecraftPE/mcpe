#pragma once
#include "world/tile/Tile.hpp"
#include "world/level/Tick.hpp"
#include "common/MovePriorityQueue.hpp"

class TileSource;

struct TickNextTickData
{
public:
	TilePos m_pos;
	TileID m_tileID;
	Tick_t m_tick;

public:
	TickNextTickData()
		: m_pos(), m_tileID(0), m_tick(0)
	{
	}
	TickNextTickData(const TilePos& pos, TileID tileID, Tick_t tick)
		: m_pos(pos), m_tileID(tileID), m_tick(tick)
	{
	}

public:
	bool operator<(const TickNextTickData& other) const
	{
		return m_tick < other.m_tick;
	}
	bool operator>(const TickNextTickData& other) const
	{
		return m_tick > other.m_tick;
	}
	bool operator==(const TickNextTickData& other) const
	{
		return m_pos == other.m_pos && m_tileID == other.m_tileID;
	}
};

class TileTickingQueue
{
private:
	Tick_t m_currentTick;
	MovePriorityQueue<TickNextTickData> m_tickData;
	bool m_instaTick;
	Random m_random;

public:
	TileTickingQueue();

public:
	void add(TileSource* region, const TilePos& pos, TileID tileID, int tickDelay);

	bool tickPendingTicks(TileSource& region, Tick_t until, int max, bool instaTick);
	void tickAllPendingTicks(TileSource& region);

	void save(CompoundTag& tag);
	void load(const CompoundTag& tag);

private:
	void _tick(TileSource& region, const TilePos& pos, TileID tileID);
};
