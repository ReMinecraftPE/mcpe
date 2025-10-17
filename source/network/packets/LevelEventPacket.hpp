#pragma once

#include "../Packet.hpp"
#include "world/level/TilePos.hpp"

class LevelEventPacket : public Packet
{
public:
	LevelEventPacket()
	{
		m_eventId = 0;
		m_data = 0;
	}
	LevelEventPacket(int16_t eventId, const TilePos& pos, int32_t data);

	void handle(const RakNet::RakNetGUID&, NetEventCallback& callback) override;
	void write(RakNet::BitStream&) override;
	void read(RakNet::BitStream&) override;
public:
	int16_t m_eventId;
	TilePos m_pos;
	int32_t m_data;
};