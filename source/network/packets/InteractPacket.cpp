#include "InteractPacket.hpp"
#include "network/NetEventCallback.hpp"

InteractPacket::InteractPacket(int32_t sourceId, int32_t targetId, int8_t actionType)
{
	m_sourceId = sourceId;
	m_targetId = targetId;
	m_actionType = actionType;
}

void InteractPacket::handle(const RakNet::RakNetGUID& guid, NetEventCallback& callback)
{
	callback.handle(guid, this);
}

void InteractPacket::write(RakNet::BitStream& bs)
{
	bs.Write((unsigned char)PACKET_INTERACT);
	bs.Write(m_actionType);
	bs.Write(m_sourceId);
	bs.Write(m_targetId);
}

void InteractPacket::read(RakNet::BitStream& bs)
{
	// m_actionType is sent last on Java
	bs.Read(m_actionType);
	bs.Read(m_sourceId);
	bs.Read(m_targetId);
}
