#pragma once

#include "Bush.hpp"

class CropsTile : public Bush
{
public:
	CropsTile(int id, int texture);

public:
	bool isSolidRender() const override;
	bool isCubeShaped() const override;
	eRenderShape getRenderShape() const override;
	void tick(Level*, const TilePos& pos, Random*) override;
	int getTexture(Facing::Name face, TileData data) const override;
	void updateShape(const LevelSource* level, const TilePos& pos) override;
	void spawnResources(Level*, const TilePos& pos, TileData data) override;

public:
	float getGrowthRate(Level* level, const TilePos& pos);
	void growCropsToMax(Level* level, const TilePos& pos);
	int getResource(TileData, Random*) const override;
};