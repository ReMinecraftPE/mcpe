#include "SetEntityDataPacket.hpp"
#include "network/NetEventCallback.hpp"
#include "network/RakIO.hpp"
#include "network/PacketUtil.hpp"

SetEntityDataPacket::SetEntityDataPacket(int32_t id, SynchedEntityData& data)
{
	m_entityId = id;
	m_bIsIncoming = false;
	m_packedItems = data.packDirty();
}

void SetEntityDataPacket::handle(const RakNet::RakNetGUID& guid, NetEventCallback& callback)
{
	callback.handle(guid, this);
}

void SetEntityDataPacket::write(RakNet::BitStream& bs)
{
	bs.Write((unsigned char)PACKET_SET_ENTITY_DATA);
	bs.Write(m_entityId);

	RakDataOutput dos(bs);
	SynchedEntityData::Pack(m_packedItems, dos);
}

void SetEntityDataPacket::read(RakNet::BitStream& bs)
{
	bs.Read(m_entityId);

	RakDataInput dis(bs);
	m_packedItems = SynchedEntityData::Unpack(dis);
	m_bIsIncoming = true;
}
