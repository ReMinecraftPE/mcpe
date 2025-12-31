/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "IceTile.hpp"
#include "world/level/Level.hpp"

IceTile::IceTile(int a, int b, Material* c) : HalfTransparentTile(a, b, c)
{
	m_friction = 0.98f;
	m_renderLayer = RENDER_LAYER_BLEND;
	setTicking(true);
}

int IceTile::getResourceCount(Random* pRandom) const
{
	return 0;
}

void IceTile::onRemove(Level* level, const TilePos& pos)
{
	Material* pMtlBelow = level->getMaterial(pos.below());
	if (pMtlBelow->blocksMotion() || pMtlBelow->isLiquid())
	{
		level->setTile(pos, Tile::water->m_ID);
	}
}

void IceTile::tick(Level* level, const TilePos& pos, Random* random)
{
	if (level->getBrightness(LightLayer::Block, pos) <= 11 - Tile::lightBlock[m_ID])
		return;

	spawnResources(level, pos, level->getData(pos));

	level->setTile(pos, Tile::calmWater->m_ID);
}
