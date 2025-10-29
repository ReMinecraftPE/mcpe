#pragma once

#include "../Packet.hpp"
#include "world/phys/Vec3.hpp"
#include "world/entity/ItemEntity.hpp"

class TakeItemEntityPacket : public Packet
{
public:
	TakeItemEntityPacket()
	{
		m_targetId = 0;
		m_sourceId = 0;
	}
	TakeItemEntityPacket(int32_t targetId, int32_t sourceId);

public:
	void handle(const RakNet::RakNetGUID&, NetEventCallback& callback) override;
	void write(RakNet::BitStream&) override;
	void read(RakNet::BitStream&) override;

public:
	int32_t m_targetId;
	int32_t m_sourceId;
};