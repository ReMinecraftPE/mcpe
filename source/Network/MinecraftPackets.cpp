#include "MinecraftPackets.hpp"

Packet* MinecraftPackets::createPacket(int type)
{
	switch (type)
	{
		case PACKET_LOGIN:
			return new LoginPacket;
		case PACKET_MESSAGE:
			return new MessagePacket;
		case PACKET_START_GAME:
			return new StartGamePacket;
		case PACKET_ADD_PLAYER:
			return new AddPlayerPacket;
		case PACKET_REMOVE_ENTITY:
			return new RemoveEntityPacket;
		case PACKET_MOVE_PLAYER:
			return new MovePlayerPacket;
		case PACKET_PLACE_BLOCK:
			return new PlaceBlockPacket;
		case PACKET_REMOVE_BLOCK:
			return new RemoveBlockPacket;
		case PACKET_UPDATE_BLOCK:
			return new UpdateBlockPacket;
		case PACKET_REQUEST_CHUNK:
			return new RequestChunkPacket;
		case PACKET_CHUNK_DATA:
			return new ChunkDataPacket;
		case PACKET_PLAYER_EQUIPMENT:
			return new PlayerEquipmentPacket;
	}

	return nullptr;
}
