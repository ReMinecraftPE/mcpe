#include "AddEntityPacket.hpp"
#include "network/NetEventCallback.hpp"
#include "../PacketUtil.hpp"

AddEntityPacket::AddEntityPacket(const Entity& entity)
	: m_entityId(entity.m_EntityID)
	, m_pos(entity.m_pos)
	, m_entityTypeId(entity.getDescriptor().getEntityType().getId())
	, m_auxValue(entity.getAuxValue())
{
	if (m_auxValue > 0)
		m_vel = entity.m_vel;
}

void AddEntityPacket::handle(const RakNet::RakNetGUID& guid, NetEventCallback& callback)
{
	callback.handle(guid, this);
}

void AddEntityPacket::write(RakNet::BitStream& bs)
{
	bs.Write((unsigned char)PACKET_ADD_ENTITY);
	bs.Write(m_entityId);
	bs.Write(m_entityTypeId);
	bs.Write(m_pos.x);
	bs.Write(m_pos.y);
	bs.Write(m_pos.z);
	bs.Write(m_auxValue);

	if (m_auxValue > 0)
	{
		int16_t motion[3];
		PacketUtil::PackMotion(m_vel, motion);
		bs.Write(motion[0]);
		bs.Write(motion[1]);
		bs.Write(motion[2]);
	}
}

void AddEntityPacket::read(RakNet::BitStream& bs)
{
	bs.Read(m_entityId);
	bs.Read(m_entityTypeId);
	bs.Read(m_pos.x);
	bs.Read(m_pos.y);
	bs.Read(m_pos.z);
	bs.Read(m_auxValue);

	if (m_auxValue > 0)
	{
		int16_t motion[3];
		bs.Read(motion[0]);
		bs.Read(motion[1]);
		bs.Read(motion[2]);

		PacketUtil::UnpackMotion(motion, m_vel);
	}
}