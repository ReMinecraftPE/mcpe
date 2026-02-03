#pragma once

#include <vector>

#include "../Packet.hpp"
#include "world/item/ItemStack.hpp"

class SendInventoryPacket : public Packet
{
public:
	SendInventoryPacket()
	{
		m_entityId = 0;
		m_count = 0;
		m_bDropAll = false;
	}
	SendInventoryPacket(int32_t entityId, const std::vector<ItemStack>& items, bool dropAll = false);

	void handle(const RakNet::RakNetGUID&, NetEventCallback& callback) override;
	void write(RakNet::BitStream&) override;
	void read(RakNet::BitStream&) override;
public:
	int32_t m_entityId;
	std::vector<ItemStack> m_items;
	uint16_t m_count;
	bool m_bDropAll;
};
