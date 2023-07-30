#include "Packet.hpp"

void MessagePacket::handle(const RakNet::RakNetGUID& guid, NetEventCallback* pCallback)
{
	pCallback->handle(guid, this);
}

void MessagePacket::write(RakNet::BitStream* bs)
{
	bs->Write(PACKET_MESSAGE);
	bs->Write(m_str);
}

void MessagePacket::read(RakNet::BitStream* bs)
{
	bs->Read(m_str);
}