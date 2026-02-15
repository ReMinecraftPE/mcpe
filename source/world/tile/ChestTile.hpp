#pragma once

#include "Tile.hpp"

class ChestTile : public Tile
{
public:
    ChestTile(int id, int texture);
public:
    int getTexture(const LevelSource*, const TilePos& pos, Facing::Name face) const override;
    int getTexture(Facing::Name face) const override;
    bool mayPlace(const Level* level, const TilePos& pos) const override;
    bool hasNeighbors(const Level* level, const TilePos& pos, int count) const;
    void onRemove(Level* level, const TilePos& pos) override;
    bool use(Level* level, const TilePos& pos, Player* var5) override;
    bool hasTileEntity() const override;
    TileEntity* newTileEntity() override;
public:
    Random m_chestRandom;
};