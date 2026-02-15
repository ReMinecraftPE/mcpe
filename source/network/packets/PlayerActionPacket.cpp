#include "PlayerActionPacket.hpp"

#include "network/NetEventCallback.hpp"

PlayerActionPacket::PlayerActionPacket(int32_t entityId, Type action, const TilePos& tilePos, Facing::Name tileFace)
	: m_entityId(entityId)
	, m_tilePos(tilePos)
	, m_tileFace(tileFace)
	, m_action(action)
{
}

void PlayerActionPacket::handle(const RakNet::RakNetGUID& guid, NetEventCallback& callback)
{
	callback.handle(guid, this);
}

void PlayerActionPacket::write(RakNet::BitStream& bs)
{
	bs.Write((unsigned char)PACKET_PLAYER_ACTION);
	bs.Write(m_action);
	bs.Write(m_tilePos.x);
	bs.Write(m_tilePos.z);
	bs.Write(m_tilePos.y);
	bs.Write(m_tileFace);
	bs.Write(m_entityId);
}

void PlayerActionPacket::read(RakNet::BitStream& bs)
{
	bs.Read(m_action);
	bs.Read(m_tilePos.x);
	bs.Read(m_tilePos.z);
	bs.Read(m_tilePos.y);
	bs.Read(m_tileFace);
	bs.Read(m_entityId);
}
