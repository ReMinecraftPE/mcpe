#pragma once

#include "Tile.hpp"

class PumpkinTile : public Tile
{
    public:
		PumpkinTile(int id, bool lantern);
		int getTexture(Facing::Name face, int data) const override;
		int getTexture(Facing::Name face) const override;
		virtual void setPlacedBy(Level*, const TilePos& pos, Mob*) override;
		bool lantern;
};

