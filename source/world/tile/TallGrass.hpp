#pragma once

#include "Bush.hpp"

class TallGrass : public Bush
{
public:
	TallGrass(TileID id, int texture);

public:
	int getResource(TileData, Random*) const override;
	bool isValidGrowTile(const TileID tile) const;
	int getColor(TileSource*, const TilePos& pos) const override;
	int getTexture(TileSource* level, const TilePos& pos, Facing::Name face) const override;
	eRenderShape getRenderShape() const override;
};
