#pragma once

#include "../Packet.hpp"

class ReadyPacket : public Packet
{
public:
	ReadyPacket(uint8_t ready = 0)
	{
		m_ready = ready;
	}

	void handle(const RakNet::RakNetGUID&, NetEventCallback& callback) override;
	void write(RakNet::BitStream&) override;
	void read(RakNet::BitStream&) override;
public:
	uint8_t m_ready;
};