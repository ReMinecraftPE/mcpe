#pragma once

#include "../Packet.hpp"

class ContainerClosePacket : public Packet
{
public:
	ContainerClosePacket()
	{
		m_containerId = 0;
	}
	ContainerClosePacket(int8_t containerId);
	
	void handle(const RakNet::RakNetGUID&, NetEventCallback& callback) override;
	void write(RakNet::BitStream&) override;
	void read(RakNet::BitStream&) override;
public:
	int8_t m_containerId;
};
