#pragma once

#include "Tile.hpp"

class FenceTile : public Tile
{
public:
	FenceTile(int id, int texture);
	bool mayPlace(const Level*, const TilePos& pos) const override;
	AABB* getAABB(const Level* pLevel, const TilePos& pos) override;
	bool isSolidRender() const override;
	bool isCubeShaped() const override;
	eRenderShape getRenderShape() const override;
};
