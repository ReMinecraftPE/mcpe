#pragma once

#include "../Packet.hpp"

class PlayerEquipmentPacket : public Packet
{
public:
	PlayerEquipmentPacket()
	{
		m_playerID = 0;
		m_itemID = 0;
	}
	PlayerEquipmentPacket(int playerID, int selectedHotbarSlot) : m_playerID(playerID), m_itemID(selectedHotbarSlot) {}
	void handle(const RakNet::RakNetGUID&, NetEventCallback& callback) override;
	void write(RakNet::BitStream&) override;
	void read(RakNet::BitStream&) override;
public:
	int m_playerID;
	uint16_t m_itemID;
};
