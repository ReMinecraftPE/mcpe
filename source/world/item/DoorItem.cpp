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

#ifdef __APPLE__
// TODO FIXME - Include source files directly.  Need to modify the Xcode project files to fix.
#include "RedstoneItem.cpp"
#endif

DoorItem::DoorItem(int id, Material* pMtl) : Item(id)
{
	m_maxStackSize = 1;
	m_maxDamage = 64;
	m_pMaterial = pMtl;
}

bool DoorItem::useOn(ItemInstance* inst, Player* player, Level* level, int x, int y, int z, int dir)
{
	if (dir != DIR_YPOS)
		return false;

	Tile* pTile = m_pMaterial == Material::wood ? Tile::door_wood : Tile::door_iron;
	if (!pTile->mayPlace(level, x, y + 1, z))
		return false;

	int faceDir = Mth::floor((((player->m_yaw + 180.0f) * 4.0f) / 360.0f) - 0.5f) & 3;
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
	int solid1 = level->isSolidTile(x - offsetX, y + 1, z - offsetZ);
	int solid2 = level->isSolidTile(x - offsetX, y + 2, z - offsetZ);
	int solid3 = level->isSolidTile(x + offsetX, y + 1, z + offsetZ);
	int solid4 = level->isSolidTile(x + offsetX, y + 2, z + offsetZ);
	int equal5 = level->getTile(x - offsetX, y + 1, z - offsetZ) == pTile->m_ID ||
	             level->getTile(x - offsetX, y + 2, z - offsetZ) == pTile->m_ID;
	int equal6 = level->getTile(x + offsetX, y + 1, z + offsetZ) == pTile->m_ID ||
	             level->getTile(x + offsetX, y + 2, z + offsetZ) == pTile->m_ID;

    if ((equal5 && !equal6) || solid2 + solid1 < solid4 + solid3)
		faceDir = 4 + ((faceDir - 1) & 3);

	// congratulations! You can now have a door.
	level->field_30 = true;
	level->setTile(x, y + 1, z, pTile->m_ID);
	level->setData(x, y + 1, z, faceDir);
	level->field_30 = false;
	level->setTile(x, y + 2, z, pTile->m_ID);
	level->setData(x, y + 2, z, faceDir + 8);
	inst->m_amount--;
	return true;
}
