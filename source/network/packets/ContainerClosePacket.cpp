#include "ContainerClosePacket.hpp"
#include "network/NetEventCallback.hpp"

ContainerClosePacket::ContainerClosePacket(int8_t containerId)
	: m_containerId(containerId)
{}

void ContainerClosePacket::handle(const RakNet::RakNetGUID& guid, NetEventCallback& callback)
{
	callback.handle(guid, this);
}

void ContainerClosePacket::write(RakNet::BitStream& bs)
{
	bs.Write((unsigned char)PACKET_CONTAINER_CLOSE);
	bs.Write(m_containerId);
}

void ContainerClosePacket::read(RakNet::BitStream& bs)
{
	bs.Read(m_containerId);
}
