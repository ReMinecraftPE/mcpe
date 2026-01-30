#include "ShovelItem.hpp"
#include "world/tile/Tile.hpp"

ShovelItem::ShovelItem(int id, Tier& tier) : ToolItem(id, 1, tier)
{
    static const Tile* shovelTiles[] =
    {
        Tile::grass,
        Tile::dirt,
        Tile::sand,
        Tile::gravel,
        Tile::topSnow,
        Tile::snow,
        Tile::clay
    };
    static const int SHOVEL_TILE_COUNT = sizeof(shovelTiles) / sizeof(Tile*);

    initializeTiles(shovelTiles, SHOVEL_TILE_COUNT);
}

bool ShovelItem::canDestroySpecial(const Tile* tile) const
{
    if (tile == Tile::topSnow)
        return true;

    return tile == Tile::snow;
}
