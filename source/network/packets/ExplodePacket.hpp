#pragma once

#include "../Packet.hpp"
#include "world/phys/Vec3.hpp"

class ExplodePacket : public Packet
{
public:
	ExplodePacket() {}
	ExplodePacket(const Vec3& pos, float range);

	void handle(const RakNet::RakNetGUID&, NetEventCallback& callback) override;
	void write(RakNet::BitStream&) override;
	void read(RakNet::BitStream&) override;
public:
	Vec3 m_pos;
	float m_range;
};