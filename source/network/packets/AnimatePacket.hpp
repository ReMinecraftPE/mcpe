#pragma once

#include "../Packet.hpp"

class AnimatePacket : public Packet
{
public:
	enum Action
	{
		NONE,
		SWING,
		HURT
	};

public:
	AnimatePacket()
	{
		m_entityId = 0;
		m_actionId = 0;
	}
	AnimatePacket(int32_t entityId, int8_t actionId);

	void handle(const RakNet::RakNetGUID&, NetEventCallback& callback) override;
	void write(RakNet::BitStream&) override;
	void read(RakNet::BitStream&) override;
public:
	int32_t m_entityId;
	int8_t m_actionId;
};