#pragma once

#include "Bush.hpp"

class TallGrass : public Bush
{
public:
	TallGrass(TileID id, int texture);
	int getResource(TileData, Random*) const override;
	bool isValidGrowTile(const TileID tile) const;
	int getColor(const LevelSource*, const TilePos& pos) const override;
	int getTexture(const LevelSource* level, const TilePos& pos, Facing::Name face) const override;
	int getRenderShape() const override;
};
