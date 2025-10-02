#pragma once

#include "../Packet.hpp"
#include "world/level/TilePos.hpp"

class RemoveBlockPacket : public Packet
{
public:
	RemoveBlockPacket()
	{
		m_entityId = 0;
	}
	RemoveBlockPacket(int id, const TilePos& pos) :m_entityId(id), m_pos(pos) {}

	void handle(const RakNet::RakNetGUID&, NetEventCallback& callback) override;
	void write(RakNet::BitStream&) override;
	void read(RakNet::BitStream&) override;
public:
	int m_entityId;
	TilePos m_pos;
};