#include "Item.hpp"
#include "TripodCamera.hpp"
#include "Level.hpp"
#include "Player.hpp"

CameraItem::CameraItem(int id) : Item(id)
{
}

ItemInstance* CameraItem::use(ItemInstance* inst, Level* level, Player* player)
{
#ifndef ORIGINAL_CODE
	// prevent players from using this in multiplayer, to prevent a desync of entity IDs
	if (level->field_11)
		return inst;
#endif

	level->addEntity(new TripodCamera(level, player, player->m_pos.x, player->m_pos.y, player->m_pos.z));
	return inst;
}
