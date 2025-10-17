#pragma once

#include "../Packet.hpp"
#include "common/Utils.hpp"
#include "world/level/TilePos.hpp"

class PlaceBlockPacket : public Packet
{
public:
	PlaceBlockPacket()
	{
		m_entityId = 0;
		m_tileTypeId = TILE_AIR;
		m_face = 0;
		m_data = 0;
	}
	PlaceBlockPacket(int entityId, const TilePos& pos, TileID tileTypeId, Facing::Name face, TileData data)
	{
		m_entityId = entityId;
		m_pos = pos;
		m_tileTypeId = tileTypeId;
		m_face = face;
		m_data = data;
	}

	void handle(const RakNet::RakNetGUID&, NetEventCallback& callback) override;
	void write(RakNet::BitStream&) override;
	void read(RakNet::BitStream&) override;
public:
	int m_entityId;
	TilePos m_pos;
	TileID m_tileTypeId;
	uint8_t m_face;
	TileData m_data;
};