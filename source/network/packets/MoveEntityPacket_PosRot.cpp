#include "MoveEntityPacket_PosRot.hpp"
#include "network/NetEventCallback.hpp"

void MoveEntityPacket_PosRot::_init()
{
	m_bHasRot = true;
}

MoveEntityPacket_PosRot::MoveEntityPacket_PosRot()
	: MoveEntityPacket()
{
	_init();
}

MoveEntityPacket_PosRot::MoveEntityPacket_PosRot(int32_t entityId, const Vec3& pos, const Vec2& rot)
{
	_init();
	m_entityId = entityId;
	m_pos = pos;
	m_rot = rot;
}

void MoveEntityPacket_PosRot::handle(const RakNet::RakNetGUID& guid, NetEventCallback& callback)
{
	callback.handle(guid, this);
}

void MoveEntityPacket_PosRot::write(RakNet::BitStream& bs)
{
	bs.Write((unsigned char)PACKET_MOVE_ENTITY_POS_ROT);
	bs.Write(m_entityId);
	bs.Write(m_pos.x);
	bs.Write(m_pos.y);
	bs.Write(m_pos.z);
	bs.Write(m_rot.x);
	bs.Write(m_rot.y);
}

void MoveEntityPacket_PosRot::read(RakNet::BitStream& bs)
{
	bs.Read(m_entityId);
	bs.Read(m_pos.x);
	bs.Read(m_pos.y);
	bs.Read(m_pos.z);
	bs.Read(m_rot.x);
	bs.Read(m_rot.y);
}