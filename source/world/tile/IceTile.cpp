/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "IceTile.hpp"
#include "world/level/TileSource.hpp"

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

void IceTile::onRemove(TileSource* source, const TilePos& pos)
{
	Material* pMtlBelow = source->getMaterial(pos.below());
	if (pMtlBelow->blocksMotion() || pMtlBelow->isLiquid())
	{
		source->setTile(pos, Tile::water->m_ID);
	}
}

void IceTile::tick(TileSource* source, const TilePos& pos, Random* random)
{
	if (source->getBrightness(LightLayer::Block, pos) <= 11 - Tile::lightBlock[m_ID])
		return;

	spawnResources(source, pos, source->getData(pos));

	source->setTile(pos, Tile::calmWater->m_ID);
}
