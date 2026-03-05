#pragma once

#include "Tile.hpp"

class CraftingTableTile : public Tile
{
public:
	CraftingTableTile(TileID id);
	bool use(TileSource*, const TilePos& pos, Player*) override;
	int getTexture(Facing::Name face) const override;
};