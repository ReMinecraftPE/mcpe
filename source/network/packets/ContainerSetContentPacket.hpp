#pragma once

#include <vector>
#include "../Packet.hpp"
#include "world/item/ItemStack.hpp"

class ContainerSetContentPacket : public Packet
{
public:
	ContainerSetContentPacket()
	{
		m_containerId = 0;
	}
	ContainerSetContentPacket(int8_t containerId, const std::vector<ItemStack>& items);

	void handle(const RakNet::RakNetGUID&, NetEventCallback& callback) override;
	void write(RakNet::BitStream&) override;
	void read(RakNet::BitStream&) override;
public:
	int8_t m_containerId;
	std::vector<ItemStack> m_items;
};
