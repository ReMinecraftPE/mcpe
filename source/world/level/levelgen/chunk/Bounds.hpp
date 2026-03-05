#pragma once
#include <cassert>

#include "world/level/TilePos.hpp"
#include "world/phys/Vec3Int32.hpp"
#include "world/level/levelgen/chunk/ChunkPos.hpp"

struct Bounds
{
public:
	class Iterator;

private:
	Vec3Int32 m_min;
	Vec3Int32 m_max;
	Vec3Int32 m_dimensions;
	int m_area;
	int m_volume;
	int m_sides;

public:
	Bounds() : m_area(0), m_volume(0), m_sides(0)
	{
	}

	// TODO: move definition to the header
	Bounds(const TilePos& min, const TilePos& max, int sides, bool noSides);

	const Vec3Int32& getMin() const
	{
		return m_min;
	}

	const Vec3Int32& getMax() const
	{
		return m_max;
	}

	const Vec3Int32& getDimensions() const
	{
		return m_dimensions;
	}

	int getArea() const
	{
		return m_area;
	}

	int getVolume() const
	{
		return m_volume;
	}

	int getSides() const
	{
		return m_sides;
	}

	Vec3 getCenter() const
	{
		return (m_min + m_max) * 0.5f;
	}

	bool contains(const Vec3Int32& pos) const
	{
		if (!isValid())
			return false;

		return pos.x >= m_min.x
			&& pos.x <= m_max.x
			&& pos.y >= m_min.y
			&& pos.y <= m_max.y
			&& pos.z >= m_min.z
			&& pos.z <= m_max.z;
	}

	bool isValid() const
	{
		return m_area > 0;
	}

	Iterator begin() const;
};

class Bounds::Iterator
{
private:
	Vec3Int32 m_pos;
	const Bounds& m_bounds;
	int m_index;

public:
	Iterator(const Bounds& bounds, int startIndex = 0)
		: m_pos(bounds.getMin()),
		  m_bounds(bounds),
		  m_index(startIndex)
	{
	}

public:
	Iterator& operator++()
	{
		m_index++;

		m_pos.x++;
		if (m_pos.x > m_bounds.getMax().x)
		{
			m_pos.z++;
			if (m_pos.z > m_bounds.getMax().z)
			{
				m_pos.y++;
			}
		}

		return *this;
	}

	const Vec3Int32& operator*() const
	{
		return m_pos;
	}

	const Vec3Int32& pos() const
	{
		return m_pos;
	}

	bool end() const
	{
		return m_index == m_bounds.getVolume();
	}

	int index() const
	{
		return m_index;
	}
};

Bounds::Iterator Bounds::begin() const
{
	return Bounds::Iterator(*this);
}
