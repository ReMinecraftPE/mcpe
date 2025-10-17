#include "TakeItemEntityPacket.hpp"
#include "network/NetEventCallback.hpp"

TakeItemEntityPacket::TakeItemEntityPacket(int32_t targetId, int32_t sourceId)
{
	m_targetId = targetId;
	m_sourceId = sourceId;
}

void TakeItemEntityPacket::handle(const RakNet::RakNetGUID& guid, NetEventCallback& callback)
{
	callback.handle(guid, this);
}

void TakeItemEntityPacket::write(RakNet::BitStream& bs)
{
	bs.Write((unsigned char)PACKET_TAKE_ITEM_ENTITY);
	bs.Write(m_targetId);
	bs.Write(m_sourceId);
}

void TakeItemEntityPacket::read(RakNet::BitStream& bs)
{
	bs.Read(m_targetId);
	bs.Read(m_sourceId);
}
