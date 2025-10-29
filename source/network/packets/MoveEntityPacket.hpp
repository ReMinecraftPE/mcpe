#pragma once

#include "../Packet.hpp"
#include "world/phys/Vec2.hpp"
#include "world/phys/Vec3.hpp"

class MoveEntityPacket : public Packet
{
public:
	MoveEntityPacket()
	{
		m_entityId = 0;
		m_bHasRot = false;
	}
	void handle(const RakNet::RakNetGUID&, NetEventCallback& callback) override;
	void write(RakNet::BitStream&) override;
	void read(RakNet::BitStream&) override;
public:
	int32_t m_entityId;
	Vec3 m_pos;
	Vec2 m_rot;
	bool m_bHasRot;
};