#pragma once

#include "../Packet.hpp"
#include "world/phys/Vec3.hpp"

class SetEntityMotionPacket : public Packet
{
public:
	SetEntityMotionPacket()
	{
		m_entityId = 0;
	}
	SetEntityMotionPacket(int32_t id, const Vec3& vel);

	void handle(const RakNet::RakNetGUID&, NetEventCallback& callback) override;
	void write(RakNet::BitStream&) override;
	void read(RakNet::BitStream&) override;
public:
	int32_t m_entityId;
	Vec3 m_vel;
};
