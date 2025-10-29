#pragma once

#include "../Packet.hpp"

class EntityEventPacket : public Packet
{
public:
	EntityEventPacket()
	{
		m_entityId = 0;
		m_eventId = 0;
	}
	EntityEventPacket(int32_t entityId, int8_t eventId);

	void handle(const RakNet::RakNetGUID&, NetEventCallback& callback) override;
	void write(RakNet::BitStream&) override;
	void read(RakNet::BitStream&) override;
public:
	int32_t m_entityId;
	int8_t m_eventId;
};