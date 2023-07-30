#include "Feature.hpp"
#include "Level.hpp"

SpringFeature::SpringFeature(int id)
{
	m_ID = id;
}

bool SpringFeature::place(Level* level, Random* random, int x, int y, int z)
{
    if (level->getTile(x, y + 1, z) != Tile::rock->m_ID)
        return false;

    if (level->getTile(x, y - 1, z) != Tile::rock->m_ID)
        return false;

    if (level->getTile(x, y, z) && level->getTile(x, y, z) != Tile::rock->m_ID)
        return false;

    int nRockTiles = 0;
    int nEmptyTiles = 0;

    if (level->getTile(x - 1, y, z) == Tile::rock->m_ID) nRockTiles++;
    if (level->getTile(x + 1, y, z) == Tile::rock->m_ID) nRockTiles++;
    if (level->getTile(x, y, z - 1) == Tile::rock->m_ID) nRockTiles++;
    if (level->getTile(x, y, z + 1) == Tile::rock->m_ID) nRockTiles++;

    if (level->isEmptyTile(x - 1, y, z)) nEmptyTiles++;
    if (level->isEmptyTile(x + 1, y, z)) nEmptyTiles++;
    if (level->isEmptyTile(x, y, z - 1)) nEmptyTiles++;
    if (level->isEmptyTile(x, y, z + 1)) nEmptyTiles++;

    if (nEmptyTiles != 1) return true;
    if (nRockTiles  != 3) return true;

    level->setTile(x, y, z, m_ID);

    level->field_10 = true;
    Tile::tiles[m_ID]->tick(level, x, y, z, random);
    level->field_10 = false;
    return true;
}
