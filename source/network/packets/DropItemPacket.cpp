#include "DropItemPacket.hpp"
#include "network/NetEventCallback.hpp"
#include "network/PacketUtil.hpp"

DropItemPacket::DropItemPacket(int32_t entityId, const ItemStack& item, bool randomly)
	: m_entityId(entityId)
	, m_bRandomly(randomly)
	, m_item(item)
{
}

void DropItemPacket::handle(const RakNet::RakNetGUID& guid, NetEventCallback& callback)
{
	callback.handle(guid, this);
}

void DropItemPacket::write(RakNet::BitStream& bs)
{
	bs.Write((unsigned char)PACKET_DROP_ITEM);
	bs.Write(m_entityId);
	bs.Write<int8_t>(m_bRandomly);
	PacketUtil::WriteItemStack(m_item, bs, false);
}

void DropItemPacket::read(RakNet::BitStream& bs)
{
	bs.Read(m_entityId);
	bs.Read((int8_t&)m_bRandomly);
	m_item = PacketUtil::ReadItemStack(bs, false);
}
