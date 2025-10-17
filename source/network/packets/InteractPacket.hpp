#pragma once

#include "../Packet.hpp"

class InteractPacket : public Packet
{
public:
	enum ActionType
	{
		NONE,
		INTERACT,
		ATTACK
	};

public:
	InteractPacket()
	{
		m_sourceId = 0;
		m_targetId = 0;
		m_actionType = INTERACT;
	}
	InteractPacket(int32_t sourceId, int32_t targetId, int8_t actionType);

	void handle(const RakNet::RakNetGUID&, NetEventCallback& callback) override;
	void write(RakNet::BitStream&) override;
	void read(RakNet::BitStream&) override;
public:
	int32_t m_sourceId;
	int32_t m_targetId;
	int8_t m_actionType;
};