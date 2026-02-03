#pragma once

#include "../Packet.hpp"
#include "world/item/ItemStack.hpp"

class DropItemPacket : public Packet
{
public:
	DropItemPacket()
	{
		m_entityId = 0;
		m_bRandomly = false;
	}
	DropItemPacket(int32_t entityId, const ItemStack& item, bool randomly = false);

	void handle(const RakNet::RakNetGUID&, NetEventCallback& callback) override;
	void write(RakNet::BitStream&) override;
	void read(RakNet::BitStream&) override;
public:
	int32_t m_entityId;
	bool m_bRandomly;
	ItemStack m_item;
};
