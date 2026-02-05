#include "ContainerSetSlotPacket.hpp"
#include "network/NetEventCallback.hpp"
#include "network/PacketUtil.hpp"

ContainerSetSlotPacket::ContainerSetSlotPacket(int8_t containerId, int16_t slot, const ItemStack& item)
	: m_containerId(containerId)
	, m_slot(slot)
	, m_item(item)
{
}

void ContainerSetSlotPacket::handle(const RakNet::RakNetGUID& guid, NetEventCallback& callback)
{
	callback.handle(guid, this);
}

void ContainerSetSlotPacket::write(RakNet::BitStream& bs)
{
	bs.Write((unsigned char)PACKET_CONTAINER_SET_SLOT);
	bs.Write(m_containerId);
	bs.Write(m_slot);
	PacketUtil::WriteItemStack(m_item, bs, false);
}

void ContainerSetSlotPacket::read(RakNet::BitStream& bs)
{
	bs.Read(m_containerId);
	bs.Read(m_slot);
	m_item = PacketUtil::ReadItemStack(bs, false);
}
