/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "CameraItem.hpp"
#include "world/level/Level.hpp"
#include "world/entity/TripodCamera.hpp"
#include "world/entity/Pig.hpp"
#include "world/entity/Player.hpp"

CameraItem::CameraItem(int id) : Item(id)
{
}

ItemInstance* CameraItem::use(ItemInstance* inst, Level* level, Player* player)
{
#ifndef ORIGINAL_CODE
	// prevent players from using this in multiplayer, to prevent a desync of entity IDs
	if (level->m_bIsMultiplayer)
		return inst;
#endif

	/*
 	Pig* pig = new Pig(level);
	pig->setPos(player->m_pos.x, player->m_pos.y, player->m_pos.z);
	level->addEntity(pig);
	*/
	
	level->addEntity(new TripodCamera(level, player, player->m_pos.x, player->m_pos.y, player->m_pos.z));
	return inst;
}
