#include "SetEntityMotionPacket.hpp"
#include "network/NetEventCallback.hpp"
#include "../PacketUtil.hpp"

SetEntityMotionPacket::SetEntityMotionPacket(int32_t id, const Vec3& vel)
	: m_entityId(id)
	, m_vel(vel)
{
}

void SetEntityMotionPacket::handle(const RakNet::RakNetGUID& guid, NetEventCallback& callback)
{
	callback.handle(guid, this);
}

void SetEntityMotionPacket::write(RakNet::BitStream& bs)
{
	int16_t motion[3];
	PacketUtil::PackMotion(m_vel, motion);

	bs.Write((unsigned char)PACKET_SET_ENTITY_MOTION);
	bs.Write(m_entityId);
	bs.Write(motion[0]);
	bs.Write(motion[1]);
	bs.Write(motion[2]);
}

void SetEntityMotionPacket::read(RakNet::BitStream& bs)
{
	bs.Read(m_entityId);

	int16_t motion[3];
	bs.Read(motion[0]);
	bs.Read(motion[1]);
	bs.Read(motion[2]);

	PacketUtil::UnpackMotion(motion, m_vel);
}
