#pragma once

#include "Tile.hpp"

class CraftingTableTile : public Tile
{
public:
	CraftingTableTile(TileID id);
	bool use(Level*, const TilePos& pos, Player*) override;
	int getTexture(Facing::Name face) const override;
};