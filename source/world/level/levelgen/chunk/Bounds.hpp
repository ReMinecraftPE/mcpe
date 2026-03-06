#pragma once
#include <cassert>

#include "world/level/TilePos.hpp"
#include "world/phys/Vec3Int32.hpp"
#include "world/level/levelgen/chunk/ChunkPos.hpp"

struct Bounds
{
public:
	struct Iterator
	{
		Iterator(const Bounds& bounds, int startIndex = 0)
			: _pos(bounds.getMin())
			, _bounds(bounds)
			, _index(startIndex)
		{
		}

		Iterator& operator++()
		{
			_index++;

			_pos.x++;
			if (_pos.x > _bounds.getMax().x)
			{
				_pos.z++;
				if (_pos.z > _bounds.getMax().z)
				{
					_pos.y++;
				}
			}

			return *this;
		}

		const Vec3Int32& operator*() const
		{
			return _pos;
		}

		const Vec3Int32& pos() const
		{
			return _pos;
		}

		bool end() const
		{
			return _index == _bounds.getVolume();
		}

		int index() const
		{
			return _index;
		}

	private:
		Vec3Int32 _pos;
		const Bounds& _bounds;
		int _index;
	};

public:
	Bounds() : _area(0), _volume(0), _sides(0)
	{
	}

	// TODO: move definition to the header
	Bounds(const TilePos& min, const TilePos& max, int sides, bool noSides);

	const Vec3Int32& getMin() const
	{
		return _min;
	}

	const Vec3Int32& getMax() const
	{
		return _max;
	}

	const Vec3Int32& getDimensions() const
	{
		return _dimensions;
	}

	int getArea() const
	{
		return _area;
	}

	int getVolume() const
	{
		return _volume;
	}

	int getSides() const
	{
		return _sides;
	}

	Vec3 getCenter() const
	{
		return (_min + _max) * 0.5f;
	}

	bool contains(const Vec3Int32& pos) const
	{
		if (!isValid())
			return false;

		return pos.x >= _min.x
			&& pos.x <= _max.x
			&& pos.y >= _min.y
			&& pos.y <= _max.y
			&& pos.z >= _min.z
			&& pos.z <= _max.z;
	}

	bool isValid() const
	{
		return _area > 0;
	}

	Iterator begin() const
	{
		return Bounds::Iterator(*this);
	}

private:
	Vec3Int32 _min;
	Vec3Int32 _max;
	Vec3Int32 _dimensions;
	int _area;
	int _volume;
	int _sides;
};
