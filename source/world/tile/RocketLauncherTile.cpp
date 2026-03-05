/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/
#include "RocketLauncherTile.hpp"
#include "world/level/Level.hpp"
#include "world/level/TileSource.hpp"
#include "world/level/TileTickingQueue.hpp"
#include "world/entity/Rocket.hpp"

RocketLauncherTile::RocketLauncherTile(TileID id) : Tile(id, 16*14+2, Material::wood)
{
	m_renderLayer = RENDER_LAYER_ALPHATEST;
	setTicking(true);
}

int RocketLauncherTile::getTexture(Facing::Name face, TileData data) const
{
	return data == 1 ? 16*14+3 : 16*14+2;
}

AABB* RocketLauncherTile::getAABB(TileSource*, const TilePos& pos)
{
	return nullptr;
}

eRenderShape RocketLauncherTile::getRenderShape() const
{
	return SHAPE_CROSS;
}

bool RocketLauncherTile::isCubeShaped() const
{
	return false;
}

bool RocketLauncherTile::isSolidRender() const
{
	return false;
}

bool RocketLauncherTile::use(const TilePos& pos, Player* player)
{
	TileSource& source = player->getTileSource();

	if (source.getData(pos) == 1)
		return true;

	source.setTileAndData(pos, FullTile(m_ID, 1));

	// spawn a rocket
	Level& level = player->getLevel();
	level.addEntity(std::make_unique<Rocket>(source, Vec3(pos) + 0.5f));

	// add a tick so that the rocket launcher will reset
	source.getTickQueue(pos)->add(&source, pos, m_ID, 10);

	return true;
}

void RocketLauncherTile::tick(TileSource* source, const TilePos& pos, Random* random)
{
	if (source->getData(pos) != 1)
		return;

	source->setTileAndData(pos, FullTile(m_ID, 0));
}
