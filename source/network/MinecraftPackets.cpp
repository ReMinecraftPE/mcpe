/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "MinecraftPackets.hpp"

Packet* MinecraftPackets::createPacket(int type)
{
	switch (type)
	{
		case PACKET_LOGIN:
			return new LoginPacket;
		case PACKET_LOGIN_STATUS:
			return new LoginStatusPacket;
		case PACKET_READY:
			return new ReadyPacket;
		case PACKET_MESSAGE:
			return new MessagePacket;
		case PACKET_SET_TIME:
			return new SetTimePacket;
		case PACKET_START_GAME:
			return new StartGamePacket;
		case PACKET_ADD_MOB:
			return new AddMobPacket;
		case PACKET_ADD_PLAYER:
			return new AddPlayerPacket;
		case PACKET_REMOVE_ENTITY:
			return new RemoveEntityPacket;
		case PACKET_ADD_ITEM_ENTITY:
			return new AddItemEntityPacket;
		case PACKET_TAKE_ITEM_ENTITY:
			return new TakeItemEntityPacket;
		case PACKET_MOVE_ENTITY:
			return new MoveEntityPacket;
		case PACKET_MOVE_ENTITY_POS_ROT:
			return new MoveEntityPacket_PosRot;
		case PACKET_MOVE_PLAYER:
			return new MovePlayerPacket;
		case PACKET_PLACE_BLOCK:
			return new PlaceBlockPacket;
		case PACKET_REMOVE_BLOCK:
			return new RemoveBlockPacket;
		case PACKET_UPDATE_BLOCK:
			return new UpdateBlockPacket;
		case PACKET_LEVEL_EVENT:
			return new LevelEventPacket;
		case PACKET_ENTITY_EVENT:
			return new EntityEventPacket;
		case PACKET_REQUEST_CHUNK:
			return new RequestChunkPacket;
		case PACKET_CHUNK_DATA:
			return new ChunkDataPacket;
		case PACKET_PLAYER_EQUIPMENT:
			return new PlayerEquipmentPacket;
		case PACKET_INTERACT:
			return new InteractPacket;
		case PACKET_USE_ITEM:
			return new UseItemPacket;
		case PACKET_SET_ENTITY_DATA:
			return new SetEntityDataPacket;
		case PACKET_SET_HEALTH:
			return new SetHealthPacket;
		case PACKET_ANIMATE:
			return new AnimatePacket;
		case PACKET_RESPAWN:
			return new RespawnPacket;

		case PACKET_LEVEL_DATA:
			return new LevelDataPacket;
	}

	return nullptr;
}
