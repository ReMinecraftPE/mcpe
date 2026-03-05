/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "DoorItem.hpp"
#include "world/level/Level.hpp"
#include "world/entity/Player.hpp"
#include "world/tile/Tile.hpp"
#include "world/level/TileSource.hpp"

DoorItem::DoorItem(int id, Material* pMtl) : Item(id)
{
	m_maxStackSize = 1;
	m_maxDamage = 64;
	m_pMaterial = pMtl;
}

bool DoorItem::useOn(ItemStack* inst, Player* player, Level* level, const TilePos& pos, Facing::Name face) const
{
	if (face != Facing::UP)
		return false;

	TileSource& source = player->getTileSource();

	Tile* pTile = m_pMaterial == Material::wood ? Tile::door_wood : Tile::door_iron;
	if (!pTile->mayPlace(&source, pos.above()))
		return false;

	int faceDir = Mth::floor((((player->m_rot.x + 180.0f) * 4.0f) / 360.0f) - 0.5f) & 3;
	int offsetX, offsetZ;
	switch (faceDir)
	{
		case 0:
			offsetX = 0;
			offsetZ = 1;
			break;
		case 1:
			offsetX = -1;
			offsetZ = 0;
			break;
		case 2:
			offsetX = 0;
			offsetZ = -1;
			break;
		case 3:
		default:
			offsetX = 1;
			offsetZ = 0;
			break;
	}

	// For polish, make sure the hinge is attached to the "correct" block
	int solid1 = source.isSolidBlockingTile(TilePos(pos.x - offsetX, pos.y + 1, pos.z - offsetZ));
	int solid2 = source.isSolidBlockingTile(TilePos(pos.x - offsetX, pos.y + 2, pos.z - offsetZ));
	int solid3 = source.isSolidBlockingTile(TilePos(pos.x + offsetX, pos.y + 1, pos.z + offsetZ));
	int solid4 = source.isSolidBlockingTile(TilePos(pos.x + offsetX, pos.y + 2, pos.z + offsetZ));
	int equal5 = source.getTile(TilePos(pos.x - offsetX, pos.y + 1, pos.z - offsetZ)) == pTile->m_ID ||
	             source.getTile(TilePos(pos.x - offsetX, pos.y + 2, pos.z - offsetZ)) == pTile->m_ID;
	int equal6 = source.getTile(TilePos(pos.x + offsetX, pos.y + 1, pos.z + offsetZ)) == pTile->m_ID ||
	             source.getTile(TilePos(pos.x + offsetX, pos.y + 2, pos.z + offsetZ)) == pTile->m_ID;

    if ((equal5 && !equal6) || solid2 + solid1 < solid4 + solid3)
		faceDir = 4 + ((faceDir - 1) & 3);

	// congratulations! You can now have a door.
	source.setTileAndData(pos.above(), FullTile(pTile->m_ID, faceDir));
	source.setTileAndData(TilePos(pos.x, pos.y + 2, pos.z), FullTile(pTile->m_ID, faceDir + 8));
	inst->m_count--;
	return true;
}
