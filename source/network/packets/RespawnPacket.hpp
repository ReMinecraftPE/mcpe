#pragma once

#include "../Packet.hpp"
#include "world/phys/Vec3.hpp"

class RespawnPacket : public Packet
{
public:
	RespawnPacket()
	{
		m_entityId = 0;
		m_pos = Vec3::ZERO;
	}
	RespawnPacket(int32_t entityId, const Vec3& pos);

	void handle(const RakNet::RakNetGUID&, NetEventCallback& callback) override;
	void write(RakNet::BitStream&) override;
	void read(RakNet::BitStream&) override;
public:
	int32_t m_entityId;
	Vec3 m_pos;
};