/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "Feature.hpp"
#include "world/level/Level.hpp"

SpringFeature::SpringFeature(TileID id)
{
	m_ID = id;
}

bool SpringFeature::place(Level* level, Random* random, const TilePos& pos)
{
    if (level->getTile(pos.above()) != Tile::rock->m_ID)
        return false;

    if (level->getTile(pos.below()) != Tile::rock->m_ID)
        return false;

    if (level->getTile(pos) && level->getTile(pos) != Tile::rock->m_ID)
        return false;

    int nRockTiles = 0;
    int nEmptyTiles = 0;

    if (level->getTile(pos.west()) == Tile::rock->m_ID) nRockTiles++;
    if (level->getTile(pos.east()) == Tile::rock->m_ID) nRockTiles++;
    if (level->getTile(pos.north()) == Tile::rock->m_ID) nRockTiles++;
    if (level->getTile(pos.south()) == Tile::rock->m_ID) nRockTiles++;

    if (level->isEmptyTile(pos.west())) nEmptyTiles++;
    if (level->isEmptyTile(pos.east())) nEmptyTiles++;
    if (level->isEmptyTile(pos.north())) nEmptyTiles++;
    if (level->isEmptyTile(pos.south())) nEmptyTiles++;

    if (nEmptyTiles != 1) return true;
    if (nRockTiles  != 3) return true;

    level->setTile(pos, m_ID);

    level->m_bInstantTicking = true;
    Tile::tiles[m_ID]->tick(level, pos, random);
    level->m_bInstantTicking = false;
    return true;
}
