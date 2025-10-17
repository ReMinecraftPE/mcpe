#include "AnimatePacket.hpp"
#include "network/NetEventCallback.hpp"

AnimatePacket::AnimatePacket(int32_t entityId, int8_t actionId)
{
	m_entityId = entityId;
	m_actionId = actionId;
}

void AnimatePacket::handle(const RakNet::RakNetGUID& guid, NetEventCallback& callback)
{
	callback.handle(guid, this);
}

void AnimatePacket::write(RakNet::BitStream& bs)
{
	bs.Write((unsigned char)PACKET_ANIMATE);
	// Mojang swapped the order on PE for fun
	bs.Write(m_actionId);
	bs.Write(m_entityId);
}

void AnimatePacket::read(RakNet::BitStream& bs)
{
	bs.Read(m_actionId);
	bs.Read(m_entityId);
}
