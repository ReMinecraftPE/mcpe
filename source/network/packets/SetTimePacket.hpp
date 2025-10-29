#pragma once

#include "../Packet.hpp"

class SetTimePacket : public Packet
{
public:
	SetTimePacket(int32_t time = 0)
	{
		m_time = time;
	}

	void handle(const RakNet::RakNetGUID&, NetEventCallback& callback) override;
	void write(RakNet::BitStream&) override;
	void read(RakNet::BitStream&) override;
public:
	int32_t m_time;
};