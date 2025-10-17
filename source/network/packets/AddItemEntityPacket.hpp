#pragma once

#include "../Packet.hpp"
#include "world/phys/Vec3.hpp"
#include "world/entity/ItemEntity.hpp"

class AddItemEntityPacket : public Packet
{
public:
	AddItemEntityPacket()
	{
		m_entityId = 0;
		m_itemId = TILE_AIR;
		m_auxValue = 0;
		m_itemCount = 0;
		m_velX = m_velY = m_velZ = 0;
	}
	AddItemEntityPacket(ItemEntity& itemEntity);

public:
	void handle(const RakNet::RakNetGUID&, NetEventCallback& callback) override;
	void write(RakNet::BitStream&) override;
	void read(RakNet::BitStream&) override;

public:
	int32_t m_entityId;
	Vec3 m_pos;
	int16_t m_itemId;
	int16_t m_auxValue;
	int8_t m_itemCount;
	int8_t m_velX;
	int8_t m_velY;
	int8_t m_velZ;
};