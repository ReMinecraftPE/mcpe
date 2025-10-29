#pragma once

#include "../Packet.hpp"
#include "world/entity/Player.hpp"

class AddPlayerPacket : public Packet
{
public:
	AddPlayerPacket()
	{
		m_id = 0;
		m_itemId = 0;
		m_itemAuxValue = 0;
	}
	AddPlayerPacket(const Player* player);
	void handle(const RakNet::RakNetGUID&, NetEventCallback& callback) override;
	void write(RakNet::BitStream&) override;
	void read(RakNet::BitStream&) override;
public:
	RakNet::RakNetGUID m_guid;
	RakNet::RakString m_name;
	int m_id;
	Vec3 m_pos;
	Vec2 m_rot;
	int16_t m_itemId;
	int16_t m_itemAuxValue;
};