/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "TntTile.hpp"
#include "world/level/Level.hpp"
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
	}

	return m_TextureFrame;
}

void TntTile::destroy(Level* level, const TilePos& pos, TileData data)
{
	// prevent players from using this in multiplayer, to prevent a desync of player IDs
	if (level->m_bIsClientSide) return;

	level->addEntity(new PrimedTnt(level, Vec3(pos) + 0.5f));
}

void TntTile::wasExploded(Level* level, const TilePos& pos)
{
	PrimedTnt* tnt = new PrimedTnt(level, Vec3(pos) + 0.5f);
	tnt->m_fuseTimer = level->m_random.nextInt(tnt->m_fuseTimer / 4) + tnt->m_fuseTimer / 8;
	level->addEntity(tnt);
}

void TntTile::neighborChanged(Level* level, const TilePos& pos, TileID tile)
{
	// @NOTE: Unused redstone
	if (tile > 0 && Tile::tiles[tile]->isSignalSource() && level->hasNeighborSignal(pos))
	{
		destroy(level, pos, 0);
		level->setTile(pos, TILE_AIR);
	}
}
