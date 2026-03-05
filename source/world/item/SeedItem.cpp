#include "SeedItem.hpp"
#include "world/level/Level.hpp"
#include "world/tile/Tile.hpp"
#include "world/level/TileSource.hpp"

SeedItem::SeedItem(int id, int place)
    : Item(id)
    , m_tile(place)
{
}

bool SeedItem::useOn(ItemStack* inst, Player* player, const TilePos& pos, Facing::Name face) const
{
    TileSource& source = player->getTileSource();

    int tile = source.getTile(pos);

    if (tile == Tile::farmland->m_ID && source.isEmptyTile(pos.above()))
    {
        source.setTile(pos.above(), m_tile);
        --inst->m_count;
        return true;
    }

    return false;
}
