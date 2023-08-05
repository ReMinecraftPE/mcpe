/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "Tile.hpp"
#include "world/level/Level.hpp"
#include "world/entity/PrimedTnt.hpp"

TntTile::TntTile(int id, int texture) : Tile(id, texture, Material::explosive)
{

}

int TntTile::getResourceCount(Random* random)
{
	return 0;
}

int TntTile::getTexture(int dir)
{
	if (dir == DIR_YNEG)
		return m_TextureFrame + 2;
	if (dir == DIR_YPOS)
		return m_TextureFrame + 1;
	
	return m_TextureFrame;
}

void TntTile::destroy(Level* level, int x, int y, int z, int data)
{
	// prevent players from using this in multiplayer, to prevent a desync of player IDs
	if (level->field_11) return;

	level->addEntity(new PrimedTnt(level, float(x) + 0.5f, float(y) + 0.5f, float(z) + 0.5f));
}

void TntTile::wasExploded(Level* level, int x, int y, int z)
{
	PrimedTnt* tnt = new PrimedTnt(level, float(x) + 0.5f, float(y) + 0.5f, float(z) + 0.5f);
	tnt->m_fuseTimer = level->field_38.nextInt(tnt->m_fuseTimer / 4) + tnt->m_fuseTimer / 8;
	level->addEntity(tnt);
}

void TntTile::neighborChanged(Level* level, int x, int y, int z, int tile)
{
	// @NOTE: Unused redstone
	if (tile > 0 && Tile::tiles[tile]->isSignalSource() && level->hasNeighborSignal(x, y, z))
	{
		destroy(level, x, y, z, 0);
		level->setTile(x, y, z, TILE_AIR);
	}
}
