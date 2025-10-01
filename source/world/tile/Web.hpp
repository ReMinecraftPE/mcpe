#pragma once

#include "Tile.hpp"

class Web : public Tile
{
public:
	Web(TileID id, int texture);

	AABB* getAABB(const Level*, const TilePos& pos) override;
	virtual int getRenderShape() const override;
	virtual bool isCubeShaped() const override;
	virtual bool isSolidRender() const override;
	void entityInside(Level*, const TilePos& pos, Entity*) const override;
	int getResource(TileData data, Random* random) const override;
};
