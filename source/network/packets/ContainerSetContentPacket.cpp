#include "ContainerSetContentPacket.hpp"
#include "network/NetEventCallback.hpp"
#include "network/PacketUtil.hpp"

ContainerSetContentPacket::ContainerSetContentPacket(int8_t containerId, const std::vector<ItemStack>& items)
	: m_containerId(containerId)
	, m_items(items)
{
}

void ContainerSetContentPacket::handle(const RakNet::RakNetGUID& guid, NetEventCallback& callback)
{
	callback.handle(guid, this);
}

void ContainerSetContentPacket::write(RakNet::BitStream& bs)
{
	bs.Write((unsigned char)PACKET_CONTAINER_SET_CONTENT);
	bs.Write(m_containerId);
	bs.Write((uint16_t)m_items.size());

	for (size_t i = 0; i < m_items.size(); i++)
	{
		PacketUtil::WriteItemStack(m_items[i], bs, false);
	}
}

void ContainerSetContentPacket::read(RakNet::BitStream& bs)
{
	bs.Read(m_containerId);
	int16_t size = 0;
	bs.Read(size);
	m_items.resize(size);

	for (uint16_t i = 0; i < size; i++)
	{
		m_items.push_back(PacketUtil::ReadItemStack(bs, false));
	}
}
