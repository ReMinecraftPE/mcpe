#pragma once

#include "Tile.hpp"

class GlowstoneTile : public Tile
{
public:
	GlowstoneTile(TileID id, int texture, Material* material);

public:
	int getResource(TileData, Random*) const override;
	int getResourceCount(Random*) const override;
};
