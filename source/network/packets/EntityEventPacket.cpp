#include "EntityEventPacket.hpp"
#include "network/NetEventCallback.hpp"

EntityEventPacket::EntityEventPacket(int32_t entityId, int8_t eventId)
{
	m_entityId = entityId;
	m_eventId = eventId;
}

void EntityEventPacket::handle(const RakNet::RakNetGUID& guid, NetEventCallback& callback)
{
	callback.handle(guid, this);
}

void EntityEventPacket::write(RakNet::BitStream& bs)
{
	bs.Write((unsigned char)PACKET_ENTITY_EVENT);
	bs.Write(m_entityId);
	bs.Write(m_eventId);
}

void EntityEventPacket::read(RakNet::BitStream& bs)
{
	bs.Read(m_entityId);
	bs.Read(m_eventId);
}
