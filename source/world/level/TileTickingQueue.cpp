#include "world/level/TileTickingQueue.hpp"
#include "world/level/TileSource.hpp"
#include "nbt/CompoundTag.hpp"
#include "nbt/ListTag.hpp"

void TileTickingQueue::_tick(TileSource& region, const TilePos& pos, TileID tileID)
{
	if (tileID == 0)
		return;

	TileID expectedTileID = region.getTile(pos);
	if (tileID == expectedTileID)
		Tile::tiles[tileID]->tick(&region, pos, &m_random);
}

void TileTickingQueue::add(TileSource* region, const TilePos& pos, TileID tileID, int tickDelay)
{
	if (!region->hasChunksAt(pos - 8, pos + 8))
		return;

	if (tickDelay < 0)
	{
		_tick(*region, pos, tileID);
	}
	else
	{
		m_tickData.push(TickNextTickData(pos, tileID, m_currentTick + tickDelay));
	}
}

bool TileTickingQueue::tickPendingTicks(TileSource& region, Tick_t until, int max, bool instaTick)
{
	m_instaTick = instaTick;

	bool hasTicked = false;
	while (!m_tickData.empty() && m_tickData.top().m_tick <= until)
	{
		TickNextTickData data;
		m_tickData.popInto(data);

		if (region.hasChunksAt(data.m_pos - 8, data.m_pos + 8))
			_tick(region, data.m_pos, data.m_tileID);

		hasTicked = true;
	}

	m_currentTick = until;
	m_instaTick = false;

	return hasTicked;
}

void TileTickingQueue::tickAllPendingTicks(TileSource& region)
{
	while (!m_tickData.empty())
	{
		tickPendingTicks(region, Tick::MAX, INT32_MAX, true);
	}
}

void TileTickingQueue::save(CompoundTag& tag)
{
	ListTag* list = new ListTag();

	for (MovePriorityQueue<TickNextTickData>::Iterator iter = m_tickData.begin(); iter != m_tickData.end(); iter++)
	{
		const TickNextTickData& data = *iter;

		CompoundTag* childTag = new CompoundTag();
		childTag->putInt32("x", data.m_pos.x);
		childTag->putInt32("y", data.m_pos.y);
		childTag->putInt32("z", data.m_pos.z);
		childTag->putInt8("tileID", static_cast<int8_t>(data.m_tileID));
		childTag->putInt64("time", static_cast<int64_t>(data.m_tick));
		list->add(childTag);
	}

	tag.put("tickList", list);
}

void TileTickingQueue::load(const CompoundTag& tag)
{
	const ListTag* list = tag.getList("tickList");

	for (size_t i = 0; i < list->size(); i++)
	{
		const CompoundTag* childTag = list->getCompound(i);

		TilePos pos = TilePos(childTag->getInt32("x"), childTag->getInt32("y"), childTag->getInt32("z"));
		TileID tileID = static_cast<TileID>(childTag->getInt8("tileID"));
		Tick_t tick = static_cast<Tick_t>(childTag->getInt64("tick"));
		m_tickData.push(TickNextTickData(pos, tileID, tick));
	}
}
