/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "LiquidTileStatic.hpp"
#include "world/level/TileSource.hpp"
#include "world/level/TileTickingQueue.hpp"

LiquidTileStatic::LiquidTileStatic(int id, Material* pMtl) : LiquidTile(id, pMtl)
{
	setTicking(false);
	if (pMtl == Material::lava)
		setTicking(true);
}

bool LiquidTileStatic::isFlammable(TileSource* source, const TilePos& pos)
{
	return source->getMaterial(pos)->isFlammable();
}

void LiquidTileStatic::neighborChanged(TileSource* source, const TilePos& pos, TileID tile)
{
	updateLiquid(source, pos);
	if (source->getTile(pos) == m_ID)
		setDynamic(source, pos);
}

void LiquidTileStatic::setDynamic(TileSource* source, const TilePos& pos)
{
	source->setTileAndDataNoUpdate(pos, FullTile(m_ID - 1, source->getData(pos)));
	//level->setTilesDirty(pos, pos);
	source->getTickQueue(pos)->add(source, pos, m_ID - 1, getTickDelay());
}

void LiquidTileStatic::tick(TileSource* source, const TilePos& pos, Random* random)
{
	if (m_pMaterial != Material::lava)
		return;

	int y2 = pos.y + random->genrand_int32() % 3;
	TilePos tp(pos);
	while (tp.y != y2)
	{
		tp.x += random->genrand_int32() % 3 - 1;
		tp.z += random->genrand_int32() % 3 - 1;

		TileID tile = source->getTile(tp.above());
		if (tile)
		{
			if (tiles[tile]->m_pMaterial->blocksMotion())
				return;
		}
		else if (
			isFlammable(source, TilePos(tp.x - 1, tp.y + 1, tp.z)) ||
			isFlammable(source, TilePos(tp.x + 1, tp.y + 1, tp.z)) ||
			isFlammable(source, TilePos(tp.x, tp.y + 1, tp.z - 1)) ||
			isFlammable(source, TilePos(tp.x, tp.y + 1, tp.z + 1)) ||
			isFlammable(source, tp) ||
			isFlammable(source, TilePos(tp.x, tp.y + 2, tp.z)))
		{
			source->setTile(tp.above(), Tile::fire->m_ID);
			return;
		}

		tp.y++;
	}
}
