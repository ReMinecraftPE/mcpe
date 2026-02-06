#include "ContainerAckPacket.hpp"
#include "network/NetEventCallback.hpp"

ContainerAckPacket::ContainerAckPacket(int8_t containerId, int16_t uid, bool accepted)
	: m_containerId(containerId)
	, m_uid(uid)
	, m_bAccepted(accepted)
{
}

void ContainerAckPacket::handle(const RakNet::RakNetGUID& guid, NetEventCallback& callback)
{
	callback.handle(guid, this);
}

void ContainerAckPacket::write(RakNet::BitStream& bs)
{
	bs.Write((unsigned char)PACKET_CONTAINER_ACK);
	bs.Write(m_containerId);
	bs.Write(m_uid);
	bs.Write(m_bAccepted);
}

void ContainerAckPacket::read(RakNet::BitStream& bs)
{
	bs.Read(m_containerId);
	bs.Read(m_uid);
	bs.Read(m_bAccepted);
}
