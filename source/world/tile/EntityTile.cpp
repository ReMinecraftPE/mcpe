#include "EntityTile.hpp"
#include "world/level/Level.hpp"

EntityTile::EntityTile(TileID id, int textureID, Material *material)  : Tile(id, textureID, material)
{
}

bool EntityTile::hasTileEntity() const
{
    return true;
}

void EntityTile::onPlace(Level *level, const TilePos &pos)
{
    Tile::onPlace(level, pos);
    level->setTileEntity(pos, newTileEntity());
}

void EntityTile::onRemove(Level *level, const TilePos &pos)
{
    Tile::onRemove(level, pos);
    level->removeTileEntity(pos);
}