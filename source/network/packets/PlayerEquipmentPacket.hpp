#pragma once

#include "../Packet.hpp"

class PlayerEquipmentPacket : public Packet
{
public:
	PlayerEquipmentPacket()
	{
		m_playerID = 0;
		m_itemID = 0;
		m_itemAuxValue = 0;
	}
	PlayerEquipmentPacket(int playerID, uint16_t itemID, uint16_t itemAuxValue)
		: m_playerID(playerID)
		, m_itemID(itemID)
		, m_itemAuxValue(itemAuxValue)
	{}
	void handle(const RakNet::RakNetGUID&, NetEventCallback& callback) override;
	void write(RakNet::BitStream&) override;
	void read(RakNet::BitStream&) override;
public:
	int m_playerID;
	uint16_t m_itemID;
	uint16_t m_itemAuxValue;
};
