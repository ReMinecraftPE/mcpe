#include "client/renderer/VisibilityExtimator.hpp"
#include "client/renderer/RenderChunk.hpp"

ThreadLocal<VisibilityExtimator> VisibilityExtimator::pool;

void VisibilityNode::connect(const ByteMask& group)
{
	for (uint8_t i = 0; i < Facing::COUNT; i++)
	{
		if (group.contains(1 << i))
		{
			connect(i, group);
		}
	}
}

void VisibilityNode::connect(uint8_t A, const ByteMask& connected)
{
	for (uint8_t i = 0; i < Facing::COUNT; i++)
	{
		if (connected.contains(1 << i))
		{
			connect(A, i);
		}
	}
}

void VisibilityNode::connect(uint8_t A, uint8_t B)
{
	if (A != B)
	{
		m_visibility[A].add(1 << B);
		m_visibility[B].add(1 << A);
	}
}

uint8_t& VisibilityExtimator::_at(const ChunkTilePos& p)
{
	size_t index = p.y + (p.x << 8) + (p.z << 4);
	assert(index < ChunkConstants::MESH_TILE_COUNT);
	return m_tiles[index];
}

uint8_t& VisibilityExtimator::_atWorld(const TilePos& t)
{
	return _at(t - m_origin);
}

uint8_t* VisibilityExtimator::_at(const ChunkTilePos& pos, ByteMask& set)
{
	if (pos.x > 128)
	{
		set.add(1 << Facing::WEST);
	}
	else if (pos.x > 15)
	{
		set.add(1 << Facing::EAST);
	}
	else if (pos.y > 128)
	{
		set.add(1 << Facing::DOWN);
	}
	else if (pos.y > 15)
	{
		set.add(1 << Facing::UP);
	}
	else if (pos.z > 128)
	{
		set.add(1 << Facing::NORTH);
	}
	else if (pos.z > 15)
	{
		set.add(1 << Facing::SOUTH);
	}
	else
	{
		return &_at(pos);
	}

	return nullptr;
}

void VisibilityExtimator::_visit(const ChunkTilePos& p, ByteMask& set)
{
	uint8_t* tileState = _at(p, set);
	if (tileState && *tileState == TS_EMPTY)
	{
		m_floodQueue.push_back(p);
	}
}

ByteMask VisibilityExtimator::_floodFill(const ChunkTilePos& startPos)
{
	ByteMask mask;

	m_floodQueue.push_back(startPos);

	while (!m_floodQueue.empty())
	{
		const ChunkTilePos& pos = m_floodQueue.front();

		uint8_t& tileState = _at(pos);
		if (tileState == TS_EMPTY)
		{
			tileState = TS_EMPTY_MARKED;

			for (uint8_t face = 0; face < Facing::COUNT; face++)
			{
				ChunkTilePos checkPos = pos + Facing::DIRECTION[face];
				_visit(checkPos, mask);
			}
		}

		m_floodQueue.pop_front();
	}

	return mask;
}

void VisibilityExtimator::setTile(const TilePos& pos, const Tile* t)
{
	if (t->isSolid())
	{
		_atWorld(pos) = TS_OPAQUE;
		m_emptyTiles--;
	}
}

void VisibilityExtimator::start(const RenderChunk& parent)
{
	m_origin = parent.getPos();

	memset(m_tiles, TS_EMPTY, ChunkConstants::MESH_TILE_COUNT);
	m_emptyTiles = ChunkConstants::MESH_TILE_COUNT;
}

VisibilityNode VisibilityExtimator::finish()
{
	if (isAllEmpty())
		return VisibilityNode(true);

	VisibilityNode node(false);

	ChunkTilePos p;
	for (p.x = 0; p.x < ChunkConstants::MESH_XYZ_SIZE; p.x++)
	{
		for (p.z = 0; p.z < ChunkConstants::MESH_XYZ_SIZE; p.z++)
		{
			for (p.y = 0; p.y < ChunkConstants::MESH_XYZ_SIZE; p.y++)
			{
				if (_at(p) != TS_EMPTY)
					continue;

				ByteMask mask = _floodFill(p);
				if (mask)
					node.connect(mask);
			}
		}
	}

	return node;
}
