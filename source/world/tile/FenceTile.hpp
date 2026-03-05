#pragma once

#include "Tile.hpp"

class FenceTile : public Tile
{
public:
	FenceTile(int id, int texture);
	bool mayPlace(TileSource*, const TilePos& pos) const override;
	AABB* getAABB(TileSource* source, const TilePos& pos) override;
	bool isSolidRender() const override;
	bool isCubeShaped() const override;
	eRenderShape getRenderShape() const override;
};
