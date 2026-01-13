#include "InteractionPacket.hpp"
#include "../NetEventCallback.hpp"

InteractionPacket::InteractionPacket(int32_t entityId, int8_t actionType, const TilePos& pos)
{
	m_entityId = entityId;
	m_actionType = actionType;
	m_pos = pos;
}

void InteractionPacket::handle(const RakNet::RakNetGUID& guid, NetEventCallback& callback)
{
	callback.handle(guid, this);
}

void InteractionPacket::write(RakNet::BitStream& bs)
{
	bs.Write((unsigned char)PACKET_INTERACTION);
	bs.Write(m_entityId);
	bs.Write(m_pos.x);
	bs.Write(m_pos.z);
	bs.Write(m_pos.y);
	bs.Write(m_actionType);
}

void InteractionPacket::read(RakNet::BitStream& bs)
{
	bs.Read(m_entityId);
	bs.Read(m_pos.x);
	bs.Read(m_pos.z);
	bs.Read(m_pos.y);
	bs.Read(m_actionType);
}
