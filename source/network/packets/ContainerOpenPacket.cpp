#include "ContainerOpenPacket.hpp"
#include "network/NetEventCallback.hpp"

ContainerOpenPacket::ContainerOpenPacket(int8_t containerId, Container::Type type, const std::string& title, int8_t size)
	: m_containerId(containerId)
	, m_type(type)
	, m_title(title.c_str())
	, m_size(size)
{}

void ContainerOpenPacket::handle(const RakNet::RakNetGUID& guid, NetEventCallback& callback)
{
	callback.handle(guid, this);
}

void ContainerOpenPacket::write(RakNet::BitStream& bs)
{
	bs.Write((unsigned char)PACKET_CONTAINER_OPEN);
	bs.Write(m_containerId);
	bs.Write((int8_t)m_type);
	bs.Write(m_size);
	bs.Write(m_title);
}

void ContainerOpenPacket::read(RakNet::BitStream& bs)
{
	bs.Read(m_containerId);
	bs.Read((int8_t&)m_type);
	bs.Read(m_size);
	bs.Read(m_title);
}
