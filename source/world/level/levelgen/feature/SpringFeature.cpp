/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "Feature.hpp"
#include "world/level/TileSource.hpp"

SpringFeature::SpringFeature(TileID id)
{
	m_ID = id;
}

bool SpringFeature::place(TileSource* source, Random* random, const TilePos& pos)
{
    if (source->getTile(pos.above()) != Tile::rock->m_ID)
        return false;

    if (source->getTile(pos.below()) != Tile::rock->m_ID)
        return false;

    if (source->getTile(pos) && source->getTile(pos) != Tile::rock->m_ID)
        return false;

    int nRockTiles = 0;
    int nEmptyTiles = 0;

    if (source->getTile(pos.west()) == Tile::rock->m_ID) nRockTiles++;
    if (source->getTile(pos.east()) == Tile::rock->m_ID) nRockTiles++;
    if (source->getTile(pos.north()) == Tile::rock->m_ID) nRockTiles++;
    if (source->getTile(pos.south()) == Tile::rock->m_ID) nRockTiles++;

    if (source->isEmptyTile(pos.west())) nEmptyTiles++;
    if (source->isEmptyTile(pos.east())) nEmptyTiles++;
    if (source->isEmptyTile(pos.north())) nEmptyTiles++;
    if (source->isEmptyTile(pos.south())) nEmptyTiles++;

    if (nEmptyTiles != 1) return true;
    if (nRockTiles  != 3) return true;

    source->setTile(pos, m_ID);

    Tile::tiles[m_ID]->tick(source, pos, random);
    return true;
}
