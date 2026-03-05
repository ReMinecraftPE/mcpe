#pragma once

#include "Tile.hpp"

class CactusTile : public Tile
{
public:
	CactusTile(int id, int texture);
	AABB* getAABB(TileSource* source, const TilePos& pos) override;
	bool mayPlace(TileSource*, const TilePos& pos) const override;
	bool canSurvive(TileSource* source, const TilePos& pos) const override;
	void neighborChanged(TileSource* source, const TilePos& pos, TileID tile) override;
	bool isSolidRender() const override;
	bool isCubeShaped() const override;
	eRenderShape getRenderShape() const override;
	void tick(TileSource*, const TilePos& pos, Random*) override;
	void entityInside(TileSource*, const TilePos& pos, Entity*) const override;
	int getTexture(Facing::Name face) const override;
	void updateShape(TileSource* source, const TilePos& pos) override;
};
