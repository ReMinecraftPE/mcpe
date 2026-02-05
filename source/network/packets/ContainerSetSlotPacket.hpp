#pragma once

#include "../Packet.hpp"
#include "world/item/ItemStack.hpp"

class ContainerSetSlotPacket : public Packet
{
public:
	ContainerSetSlotPacket()
	{
		m_containerId = 0;
		m_slot = 0;
	}
	ContainerSetSlotPacket(int8_t containerId, int16_t slot, const ItemStack& item);

	void handle(const RakNet::RakNetGUID&, NetEventCallback& callback) override;
	void write(RakNet::BitStream&) override;
	void read(RakNet::BitStream&) override;
public:
	int8_t m_containerId;
	int16_t m_slot;
	ItemStack m_item;
};
