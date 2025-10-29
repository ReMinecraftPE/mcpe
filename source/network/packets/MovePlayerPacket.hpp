#pragma once

#include "../Packet.hpp"
#include "world/phys/Vec2.hpp"
#include "world/phys/Vec3.hpp"

class MovePlayerPacket : public Packet
{
public:
	MovePlayerPacket()
	{
		m_id = 0;
	}
	MovePlayerPacket(int id, const Vec3& pos, const Vec2& rot) : m_id(id), m_pos(pos), m_rot(rot) {}
	void handle(const RakNet::RakNetGUID&, NetEventCallback& callback) override;
	void write(RakNet::BitStream&) override;
	void read(RakNet::BitStream&) override;
public:
	int m_id;
	Vec3 m_pos;
	Vec2 m_rot;
};