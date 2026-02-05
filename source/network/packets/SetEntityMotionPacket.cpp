#include "SetEntityMotionPacket.hpp"
#include "network/NetEventCallback.hpp"

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
	int16_t x = Mth::clamp(m_vel.x, -3.9f, 3.9f) * 8000.0f;
	int16_t y = Mth::clamp(m_vel.y, -3.9f, 3.9f) * 8000.0f;
	int16_t z = Mth::clamp(m_vel.z, -3.9f, 3.9f) * 8000.0f;

	bs.Write((unsigned char)PACKET_SET_ENTITY_MOTION);
	bs.Write(m_entityId);
	bs.Write(x);
	bs.Write(y);
	bs.Write(z);
}

void SetEntityMotionPacket::read(RakNet::BitStream& bs)
{
	bs.Read(m_entityId);

	int16_t x, y, z;
	bs.Read(x);
	bs.Read(y);
	bs.Read(z);

	m_vel.x = (float)x / 8000.0f;
	m_vel.y = (float)y / 8000.0f;
	m_vel.z = (float)z / 8000.0f;
}
