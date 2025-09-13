#pragma once

#include "Bush.hpp"

class TallGrass : public Bush
{
public:
	TallGrass(int id, int texture);
	int getResource(int, Random*) const override;
	bool isValidGrowTile(const TileID tile) const;
	int getColor(const LevelSource*, const TilePos& pos) const override;
	int getTexture(const LevelSource* level, const TilePos& pos, Facing::Name face) const override;
	virtual int getRenderShape() const override;
};
