#include "SetHealthPacket.hpp"
#include "network/NetEventCallback.hpp"

void SetHealthPacket::handle(const RakNet::RakNetGUID& guid, NetEventCallback& callback)
{
	callback.handle(guid, this);
}

void SetHealthPacket::write(RakNet::BitStream& bs)
{
	bs.Write((unsigned char)PACKET_SET_HEALTH);
	bs.Write(m_health);
}

void SetHealthPacket::read(RakNet::BitStream& bs)
{
	bs.Read(m_health);
}
