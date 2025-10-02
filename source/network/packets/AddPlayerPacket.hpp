#pragma once

#include "../Packet.hpp"
#include "world/entity/Player.hpp"

class AddPlayerPacket : public Packet
{
public:
	AddPlayerPacket()
	{
		field_4 = 0;
		field_14 = 0;
		m_id = 0;
	}
	AddPlayerPacket(const Player* player);
	void handle(const RakNet::RakNetGUID&, NetEventCallback& callback) override;
	void write(RakNet::BitStream&) override;
	void read(RakNet::BitStream&) override;
public:
	int field_4;
	RakNet::RakNetGUID m_guid;
	int field_14;
	RakNet::RakString m_name;
	int m_id;
	Vec3 m_pos;
};