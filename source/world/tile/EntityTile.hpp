#pragma once

#include "Tile.hpp"

class EntityTile : public Tile
{
public:
    EntityTile(TileID id, int textureID, Material* material);

public:
    virtual TileEntity* newTileEntity() = 0;

public:
	bool hasTileEntity() const override;
    void onPlace(Level* level, const TilePos& pos) override;
	void onRemove(Level* level, const TilePos& pos) override;
};