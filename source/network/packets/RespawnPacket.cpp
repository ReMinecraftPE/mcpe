#include "RespawnPacket.hpp"
#include "network/NetEventCallback.hpp"

RespawnPacket::RespawnPacket(int32_t entityId, const Vec3& pos)
{
	m_entityId = entityId;
	m_pos = pos;
}

void RespawnPacket::handle(const RakNet::RakNetGUID& guid, NetEventCallback& callback)
{
	callback.handle(guid, this);
}

void RespawnPacket::write(RakNet::BitStream& bs)
{
	bs.Write((unsigned char)PACKET_RESPAWN);
	bs.Write(m_entityId);
	bs.Write(m_pos.x);
	bs.Write(m_pos.y);
	bs.Write(m_pos.z);
}

void RespawnPacket::read(RakNet::BitStream& bs)
{
	bs.Read(m_entityId);
	bs.Read(m_pos.x);
	bs.Read(m_pos.y);
	bs.Read(m_pos.z);
}
