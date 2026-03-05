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
#include "world/entity/Chicken.hpp"
#include "world/entity/Cow.hpp"
#include "world/entity/Player.hpp"

CameraItem::CameraItem(int id) : Item(id)
{
}

ItemStack* CameraItem::use(ItemStack* inst, Mob* user) const
{
	Level& level = user->getLevel();

#ifndef ORIGINAL_CODE
	// prevent players from using this in multiplayer, to prevent a desync of entity IDs
	if (level.m_bIsClientSide)
		return inst;
#endif

	if (!user->isPlayer())
		return inst;
	
	Player* player = static_cast<Player*>(user);
	level.addEntity(new TripodCamera(user->getTileSource(), player, player->m_pos));
	return inst;
}
