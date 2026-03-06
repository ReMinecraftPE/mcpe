#pragma once
#include <vector>
#include <functional>
#include "world/level/levelgen/chunk/Bounds.hpp"
#include "world/level/levelgen/chunk/ChunkPos.hpp"

template<typename T>
class GridArea
{
public:
	GridArea()
		: m_circular(false)
	{
	}

	GridArea(
		const TilePos& min,
		const TilePos& max,
		int sides,
		const std::function<T(const Vec3Int32&)>& generate,
		const std::function<void(T&)>& clearCallback,
		bool circular,
		const std::function<void(T&)>& addCallback,
		const std::function<void(T&)>& moveCallback)
		: m_generate(generate),
		  m_clearCallback(clearCallback),
		  m_addCallback(addCallback),
		  m_moveCallback(moveCallback),
		  m_bounds(min, max, sides, false),
		  m_circular(circular)
	{
		m_chunks.resize(m_bounds.getVolume());

		if (m_generate)
			_fill();
	}

	MC_CTOR_MOVE(GridArea);

	~GridArea()
	{
		clear();
	}

public:
	void move(const TilePos& tileMin, const TilePos& tileMax)
	{
		Bounds newBounds(tileMin, tileMax, m_bounds.getSides(), false);

		m_newChunks.resize(newBounds.getVolume());

		_moveOldChunks(newBounds);

		if (m_clearCallback)
		{
			_callback(m_moveCallback);
			_callback(m_clearCallback);
		}

		m_bounds = newBounds;
		m_chunks = m_newChunks;

		if (m_generate)
			_fill();
	}

	void clear()
	{
		_callback(m_clearCallback);

		m_bounds = Bounds();
		m_chunks.clear();
	}

	const T& at(const Vec3Int32& pos) const
	{
		assert(contains(pos));
		return m_chunks[_getChunkIndex(m_bounds, pos)];
	}

	bool contains(const Vec3Int32& pos) const
	{
		if (m_chunks.empty())
			return false;

		return m_bounds.contains(pos);
	}

	const Bounds& getBounds() const
	{
		return m_bounds;
	}

	bool empty() const
	{
		return m_chunks.empty();
	}

	MC_FUNC_MOVE(GridArea);

private:
	static int _getChunkIndex(const Bounds& bounds, const Vec3Int32& pos)
	{
		return (pos.x - bounds.getMin().x) + bounds.getDimensions().x * (pos.z - bounds.getMin().z) + bounds.getArea() * (pos.y - bounds.getMin().y);
	}

	void _callback(const std::function<void(T&)>& callback)
	{
		if (callback)
		{
			for (typename std::vector<T>::iterator iter = m_chunks.begin(); iter != m_chunks.end(); iter++)
			{
				T& chunk = *iter;
				if (chunk)
					callback(chunk);
			}
		}
	}

	void _fill()
	{
		Bounds::Iterator iter = m_bounds.begin();

		if (m_circular)
		{
			float radius = m_bounds.getDimensions().x * 0.5f - 0.45f;
			Vec3 center = m_bounds.getCenter();

			while (!iter.end())
			{
				int index = iter.index();
				const Vec3Int32& pos = iter.pos();

				T& chunk = m_chunks[index];
				if (!chunk)
				{
					if ((radius * radius) > center.distanceToSqr(pos.toVec3()))
					{
						chunk = m_generate(pos);

						if (m_addCallback)
							m_addCallback(chunk);
					}
				}

				++iter;
			}
		}
		else
		{
			while (!iter.end())
			{
				int index = iter.index();
				const Vec3Int32& pos = iter.pos();

				T& chunk = m_chunks[index];
				if (!chunk)
				{
					chunk = m_generate(pos);

					if (m_addCallback)
						m_addCallback(chunk);
				}

				++iter;
			}
		}
	}

	void _moveOldChunks(const Bounds& newBounds)
	{
		Bounds::Iterator iter = m_bounds.begin();

		if (m_circular)
		{
			float radius = m_bounds.getDimensions().x * 0.5f - 0.45f;
			Vec3 center = m_bounds.getCenter();

			while (!iter.end())
			{
				int index = iter.index();
				const Vec3Int32& pos = iter.pos();

				T& chunk = m_chunks[index];
				if (chunk)
				{
					if (newBounds.contains(pos) && (radius * radius) > center.distanceToSqr(Vec3(pos.x, pos.y, pos.z)))
					{
						m_newChunks[_getChunkIndex(newBounds, pos)] = chunk;
						m_chunks[index] = nullptr;
					}
				}

				++iter;
			}
		}
		else
		{
			while (!iter.end())
			{
				int index = iter.index();
				const Vec3Int32& pos = iter.pos();

				T& chunk = m_chunks[index];
				if (chunk)
				{
					if (newBounds.contains(pos))
					{
						m_newChunks[_getChunkIndex(newBounds, pos)] = chunk;
						m_chunks[index] = nullptr;
					}
				}

				++iter;
			}
		}
	}

	void _move(GridArea& other)
	{
		clear();
		
		// TODO: the rest
	}

private:
	std::function<void(T&)> m_clearCallback;
	std::function<T(const Vec3Int32&)> m_generate;
	std::function<void(T&)> m_addCallback;
	std::function<void(T&)> m_moveCallback;
	Bounds m_bounds;
	std::vector<T> m_chunks;
	std::vector<T> m_newChunks;
	bool m_circular;
};
