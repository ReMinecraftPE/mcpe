#include "SendInventoryPacket.hpp"
#include "network/NetEventCallback.hpp"
#include "network/PacketUtil.hpp"

SendInventoryPacket::SendInventoryPacket(int32_t entityId, const std::vector<ItemStack>& items, bool dropAll)
	: m_entityId(entityId)
	, m_items(items)
	, m_count(0)
	, m_bDropAll(dropAll)
{
}

void SendInventoryPacket::handle(const RakNet::RakNetGUID& guid, NetEventCallback& callback)
{
	callback.handle(guid, this);
}

void SendInventoryPacket::write(RakNet::BitStream& bs)
{
	bs.Write((unsigned char)PACKET_DROP_ITEM);
	bs.Write(m_entityId);
	bs.Write(m_bDropAll);
	bs.Write((uint16_t)m_items.size());

	for (size_t i = 0; i < m_items.size(); i++)
	{
		PacketUtil::WriteItemStack(m_items[i], bs, false);
	}
}

void SendInventoryPacket::read(RakNet::BitStream& bs)
{
	bs.Read(m_entityId);
	bs.Read(m_bDropAll);
	bs.Read(m_count);

	// End early, processing the rest of this (like PE does) is just a recipe for disaster
	// Plus, it doesn't really matter, as PE just sends 512 empty items anyways
	/*
	m_items.resize(m_count);
	for (uint16_t i = 0; i < m_count; i++)
	{
		m_items.push_back(PacketUtil::ReadItemStack(bs, false));
	}*/
}
