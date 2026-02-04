#pragma once

#include "../Packet.hpp"

class ContainerAckPacket : public Packet
{
public:
	ContainerAckPacket()
	{
		m_containerId = 0;
		m_uid = 0;
		m_bAccepted = false;
	}

	ContainerAckPacket(int8_t containerId, int16_t uid, bool accepted);

	void handle(const RakNet::RakNetGUID&, NetEventCallback& callback) override;
	void write(RakNet::BitStream&) override;
	void read(RakNet::BitStream&) override;
public:
	int8_t m_containerId;
	int16_t m_uid;
	bool m_bAccepted;
};
