#pragma once

#include "../Packet.hpp"
#include "world/phys/Vec3.hpp"

class Entity;

class AddEntityPacket : public Packet
{
public:
	AddEntityPacket()
	{
		m_entityId = 0;
		m_entityTypeId = 0;
		m_auxValue = 0;
	}
	AddEntityPacket(const Entity& entity);

public:
	void handle(const RakNet::RakNetGUID&, NetEventCallback& callback) override;
	void write(RakNet::BitStream&) override;
	void read(RakNet::BitStream&) override;

public:
	int32_t m_entityId;
	Vec3 m_pos;
	Vec3 m_vel;
	int8_t m_entityTypeId;
	int32_t m_auxValue;
};
