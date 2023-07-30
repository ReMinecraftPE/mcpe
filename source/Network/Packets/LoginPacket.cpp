#include "Packet.hpp"

void LoginPacket::handle(const RakNet::RakNetGUID& guid, NetEventCallback* pCallback)
{
	pCallback->handle(guid, this);
}

void LoginPacket::write(RakNet::BitStream* bs)
{
	bs->Write(PACKET_LOGIN);
	bs->Write(m_str);
}

void LoginPacket::read(RakNet::BitStream* bs)
{
	bs->Read(m_str);
}
