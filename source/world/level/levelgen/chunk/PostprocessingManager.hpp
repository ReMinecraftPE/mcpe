#pragma once
#include <set>
#include <memory>
#include <mutex>
#include "world/level/levelgen/chunk/ChunkPos.hpp"
#include "compat/LegacyCPP.hpp"

class PostprocessingManager
{
	class ScopedLock;
	friend class ScopedLock;

private:
	static PostprocessingManager instance;

public:
	static PostprocessingManager& getInstance()
	{
		return instance;
	}

protected:
	bool m_canLock;
	std::mutex m_mutex;
	std::set<ChunkPos> m_lockedPositions;

public:
	PostprocessingManager();

public:
	ScopedLock tryLock(const ChunkPos&);

protected:
	void _release(const ChunkPos&);

public:
	class ScopedLock
	{
		friend class PostprocessingManager;

	protected:
		ChunkPos m_lockedPosition;

	private:
		// disable the copy operator
		ScopedLock& operator=(const ScopedLock&);

	protected:
		ScopedLock(const ChunkPos& pos)
			: m_lockedPosition(pos)
		{
		}

	public:
		MC_CTOR_MOVE(ScopedLock);
		~ScopedLock()
		{
			if (isValid())
				instance._release(m_lockedPosition);
		}

	public:
		bool isValid() const
		{
			return m_lockedPosition != ChunkPos::INVALID;
		}

	protected:
		void _move(ScopedLock& other)
		{
			m_lockedPosition = other.m_lockedPosition;
			other.m_lockedPosition = ChunkPos::INVALID;
		}
	};
};
