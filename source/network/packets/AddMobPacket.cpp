#include "AddMobPacket.hpp"
#include "network/NetEventCallback.hpp"
#include "network/RakIO.hpp"
#include "network/PacketUtil.hpp"
#include "world/entity/Mob.hpp"

AddMobPacket::AddMobPacket(const Mob& mob)
{
	m_entityId = mob.m_EntityID;
	m_entityTypeId = mob.getDescriptor().getEntityType().getId();
	m_pos = mob.m_pos;
	m_rot = mob.m_rot;
	m_entityData = mob.getEntityData();
}

void AddMobPacket::handle(const RakNet::RakNetGUID& guid, NetEventCallback& callback)
{
	callback.handle(guid, this);
}

void AddMobPacket::write(RakNet::BitStream* bs)
{
	bs->Write((unsigned char)PACKET_ADD_MOB);
	bs->Write(m_entityId);
	bs->Write(m_entityTypeId);
	bs->Write(m_pos.x);
	bs->Write(m_pos.y);
	bs->Write(m_pos.z);
	bs->Write(PacketUtil::Rot_degreesToChar(m_rot.y));
	bs->Write(PacketUtil::Rot_degreesToChar(m_rot.x));

	/*RakDataOutput dos(*bs);
	m_entityData.packAll(dos);*/
}

void AddMobPacket::read(RakNet::BitStream* bs)
{
	bs->Read(m_entityId);
	bs->Read(m_entityTypeId);
	bs->Read(m_pos.x);
	bs->Read(m_pos.y);
	bs->Read(m_pos.z);

	char yaw, pitch;
	bs->Read(yaw);
	bs->Read(pitch);

	/*RakDataInput dis(*bs);
	m_entityData = SynchedEntityData::Unpack(dis);*/

	m_rot.x = PacketUtil::Rot_charToDegrees(pitch);
	m_rot.y = PacketUtil::Rot_charToDegrees(yaw);
}
