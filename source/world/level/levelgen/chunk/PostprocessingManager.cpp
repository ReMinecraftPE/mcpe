#include <thread>
#include <cassert>
#include "world/level/levelgen/chunk/PostprocessingManager.hpp"

PostprocessingManager PostprocessingManager::instance;

PostprocessingManager::PostprocessingManager()
	: m_canLock(std::thread::hardware_concurrency() > 2) // We don't want to lock if we don't have multiple streaming threads running
{
}

PostprocessingManager::ScopedLock PostprocessingManager::tryLock(const ChunkPos& pos)
{
	assert(pos != ChunkPos::INVALID);

	if (!m_canLock)
		return ScopedLock(pos);

	std::lock_guard<std::mutex> lock(m_mutex);

	const ChunkPos minPos = pos - 1;
	const ChunkPos maxPos = pos + 1;

	if (!m_lockedPositions.empty())
	{
		for (int x = minPos.x; x <= maxPos.x; x++)
		{
			for (int z = minPos.z; z <= maxPos.z; z++)
			{
				ChunkPos pos(x, z);
				if (m_lockedPositions.find(pos) != m_lockedPositions.end())
					return ScopedLock(ChunkPos::INVALID);
			}
		}
	}

	for (int x = minPos.x; x <= maxPos.x; x++)
	{
		for (int z = minPos.z; z <= maxPos.z; z++)
		{
			ChunkPos pos(x, z);
			m_lockedPositions.insert(pos);
		}
	}

	return ScopedLock(pos);
}

void PostprocessingManager::_release(const ChunkPos& pos)
{
	assert(pos != ChunkPos::INVALID);

	if (!m_canLock)
		return;

	std::lock_guard<std::mutex> lock(m_mutex);

	const ChunkPos minPos = pos - 1;
	const ChunkPos maxPos = pos + 1;

	for (int x = minPos.x; x <= maxPos.x; x++)
	{
		for (int z = minPos.z; z <= maxPos.z; z++)
		{
			ChunkPos pos(x, z);
			std::set<ChunkPos>::iterator iter = m_lockedPositions.find(pos);
			assert(iter != m_lockedPositions.end());
			m_lockedPositions.erase(iter);
		}
	}
}
