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
	void tick(TileSource*, const TilePos& pos, Random*) override;
	int getTexture(Facing::Name face, TileData data) const override;
	void updateShape(TileSource* source, const TilePos& pos) override;
	void spawnResources(TileSource*, const TilePos& pos, TileData data) override;
	void neighborChanged(TileSource* source, const TilePos& pos, TileID tile);

public:
	float getGrowthRate(TileSource* source, const TilePos& pos);
	void growCropsToMax(TileSource* source, const TilePos& pos);
	int getResource(TileData, Random*) const override;
};