#include "UseItemPacket.hpp"
#include "network/NetEventCallback.hpp"

UseItemPacket::UseItemPacket(const TilePos& tilePos, int32_t tileFace, int32_t entityId, const ItemInstance* pItem)
{
	m_tilePos = tilePos;
	m_tileFace = tileFace;
	m_entityId = entityId;
	if (pItem)
	{
		m_itemId = pItem->getId();
		m_itemAuxValue = pItem->getAuxValue();
	}
	else
	{
		m_itemId = TILE_AIR;
		m_itemAuxValue = 0;
	}
}

void UseItemPacket::handle(const RakNet::RakNetGUID& guid, NetEventCallback& callback)
{
	callback.handle(guid, this);
}

void UseItemPacket::write(RakNet::BitStream& bs)
{
	bs.Write((unsigned char)PACKET_USE_ITEM);
	bs.Write(m_tilePos.x);
	bs.Write(m_tilePos.y);
	bs.Write(m_tilePos.z);
	bs.Write(m_tileFace);
	bs.Write(m_itemId);
	bs.Write(m_itemAuxValue);
	bs.Write(m_entityId);
}

void UseItemPacket::read(RakNet::BitStream& bs)
{
	bs.Read(m_tilePos.x);
	bs.Read(m_tilePos.y);
	bs.Read(m_tilePos.z);
	bs.Read(m_tileFace);
	bs.Read(m_itemId);
	bs.Read(m_itemAuxValue);
	bs.Read(m_entityId);

	m_item = ItemInstance(m_itemId, m_itemId > 0 ? 1 : 0, m_itemAuxValue);
}
