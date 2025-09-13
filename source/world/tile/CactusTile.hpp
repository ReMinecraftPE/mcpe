#pragma once

#include "Tile.hpp"

class CactusTile : public Tile
{
public:
	CactusTile(int id, int texture);
	AABB* getAABB(const Level* pLevel, const TilePos& pos) override;
	bool mayPlace(const Level*, const TilePos& pos) const override;
	bool canSurvive(const Level* level, const TilePos& pos) const override;
	void neighborChanged(Level* level, const TilePos& pos, TileID tile) override;
	bool isSolidRender() const override;
	bool isCubeShaped() const override;
	int getRenderShape() const override;
	void tick(Level*, const TilePos& pos, Random*) override;
	void entityInside(Level*, const TilePos& pos, Entity*) const override;
	int getTexture(Facing::Name face) const override;
	void updateShape(const LevelSource* level, const TilePos& pos) override;
};
