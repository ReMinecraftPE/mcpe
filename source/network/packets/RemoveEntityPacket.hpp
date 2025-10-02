#pragma once

#include "../Packet.hpp"

class RemoveEntityPacket : public Packet
{
public:
	RemoveEntityPacket()
	{
		m_id = 0;
	}
	RemoveEntityPacket(int id) { m_id = id; }

	void handle(const RakNet::RakNetGUID&, NetEventCallback& callback) override;
	void write(RakNet::BitStream&) override;
	void read(RakNet::BitStream&) override;
public:
	int m_id;
};