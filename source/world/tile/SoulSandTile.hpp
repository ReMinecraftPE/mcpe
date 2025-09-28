#pragma once

#include "Tile.hpp"

class SoulSandTile : public Tile
{
public:
	SoulSandTile(int id, int texture);
	void entityInside(Level* level, const TilePos& pos, Entity* entity) const override;
	AABB* getAABB(const Level* pLevel, const TilePos& pos) override;
};
