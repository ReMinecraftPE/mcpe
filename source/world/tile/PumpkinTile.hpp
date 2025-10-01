#pragma once

#include "Tile.hpp"

class PumpkinTile : public Tile
{
    public:
		PumpkinTile(TileID id, bool lantern);
		int getTexture(Facing::Name face, TileData data) const override;
		int getTexture(Facing::Name face) const override;
		void setPlacedBy(Level*, const TilePos& pos, Mob*) override;
		bool m_bLantern;
};
