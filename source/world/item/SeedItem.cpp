#include "SeedItem.hpp"
#include "world/level/Level.hpp"
#include "world/tile/Tile.hpp"

SeedItem::SeedItem(int id, int place) : Item(id), m_tile(place)
{
}

bool SeedItem::useOn(ItemStack* inst, Player* player, Level* level, const TilePos& pos, Facing::Name face) const
{
    int tile = level->getTile(pos);

    if (tile == Tile::farmland->m_ID && level->isEmptyTile(pos.above()))
    {
        level->setTile(pos.above(), m_tile);
        --inst->m_count;
        return true;
    }

    return false;
}
