#pragma once

#include "../Packet.hpp"

class SetHealthPacket : public Packet
{
public:
	SetHealthPacket(int8_t health = 0)
	{
		m_health = health;
	}

	void handle(const RakNet::RakNetGUID&, NetEventCallback& callback) override;
	void write(RakNet::BitStream&) override;
	void read(RakNet::BitStream&) override;
public:
	int8_t m_health; // int16_t in Java
};