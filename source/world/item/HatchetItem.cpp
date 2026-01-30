#include "HatchetItem.hpp"
#include "world/tile/Tile.hpp"

HatchetItem::HatchetItem(int id, Tier& tier) : ToolItem(id, 1, tier)
{
    static const Tile* hatchetTiles[] =
    {
        Tile::wood,
        Tile::bookshelf,
        Tile::treeTrunk,
        // Tile::chest
    };

    static const int HATCHET_TILE_COUNT = sizeof(hatchetTiles) / sizeof(Tile*);
    initializeTiles(hatchetTiles, HATCHET_TILE_COUNT);
}