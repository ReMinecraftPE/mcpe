#pragma once

#include "EntityTile.hpp"

class MusicTile : public EntityTile
{
public:
    MusicTile(TileID id, int textureID);

public:
	void neighborChanged(Level*, const TilePos& pos, TileID tile) override;
	bool use(Level*, const TilePos&, Player*) override;
	void attack(Level*, const TilePos&, Player*) override;
	bool hasTileEntity() const override;
	void triggerEvent(Level*, const TileEvent&) override;
	TileEntity* newTileEntity() override;
};