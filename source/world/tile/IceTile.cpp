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
	setTicking(true);
}

int IceTile::getRenderLayer() const
{
	return LAYER_ALPHA;
}

int IceTile::getResourceCount(Random* pRandom) const
{
	return 0;
}

void IceTile::onRemove(Level* level, int x, int y, int z)
{
	Material* pMtlBelow = level->getMaterial(x, y - 1, z);
	if (pMtlBelow->blocksMotion() || pMtlBelow->isLiquid())
	{
		level->setTile(x, y, z, Tile::water->m_ID);
	}
}

bool IceTile::shouldRenderFace(const LevelSource* level, int x, int y, int z, int dir) const
{
	//@BUG: 1 - dir? This would only work to flip the YNEG and YPOS directions.
	return HalfTransparentTile::shouldRenderFace(level, x, y, z, 1 - dir);
}

void IceTile::tick(Level* level, int x, int y, int z, Random* random)
{
	if (level->getBrightness(LightLayer::Block, x, y, z) <= 11 - Tile::lightBlock[m_ID])
		return;

	spawnResources(level, x, y, z, level->getData(x, y, z));

	level->setTile(x, y, z, Tile::calmWater->m_ID);
}
