#pragma once

#include "Tile.hpp"

class GlowstoneTile : public Tile
{
public:
	GlowstoneTile(int id, int texture, Material* material);

	int getResource(int, Random*) const override;
	int getResourceCount(Random*) const override;
};