/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "TntTile.hpp"
#include "world/level/Level.hpp"
#include "world/level/TileSource.hpp"
#include "world/entity/PrimedTnt.hpp"

TntTile::TntTile(int id, int texture) : Tile(id, texture, Material::explosive)
{

}

int TntTile::getResourceCount(Random* random) const
{
	return 0;
}

int TntTile::getTexture(Facing::Name face) const
{
	switch (face)
	{
	case Facing::DOWN:
		return m_TextureFrame + 2;
	case Facing::UP:
		return m_TextureFrame + 1;
	default:
		return m_TextureFrame;
	}
}

void TntTile::destroy(TileSource* source, const TilePos& pos, TileData data)
{
	// prevent players from using this in multiplayer, to prevent a desync of player IDs
	Level& level = source->getLevel();
	if (level.m_bIsClientSide) return;

	level.addEntity(std::make_unique<PrimedTnt>(*source, Vec3(pos) + 0.5f));
}

void TntTile::wasExploded(TileSource* source, const TilePos& pos)
{
	Level& level = source->getLevel();

	PrimedTnt* tnt = new PrimedTnt(*source, Vec3(pos) + 0.5f);
	tnt->m_fuseTimer = level.m_random.nextInt(tnt->m_fuseTimer / 4) + tnt->m_fuseTimer / 8;
	level.addEntity(tnt);
}

void TntTile::neighborChanged(TileSource* source, const TilePos& pos, TileID tile)
{
	// @NOTE: Unused redstone
	if (tile > 0 && Tile::tiles[tile]->isSignalSource() && source->hasNeighborSignal(pos))
	{
		destroy(source, pos, 0);
		source->setTile(pos, TILE_AIR);
	}
}
