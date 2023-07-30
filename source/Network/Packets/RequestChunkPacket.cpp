#include "Packet.hpp"

void RequestChunkPacket::handle(const RakNet::RakNetGUID& guid, NetEventCallback* pCallback)
{
	pCallback->handle(guid, this);
}

void RequestChunkPacket::write(RakNet::BitStream* bs)
{
	bs->Write(PACKET_REQUEST_CHUNK);
	bs->Write(m_x);
	bs->Write(m_z);
}

void RequestChunkPacket::read(RakNet::BitStream* bs)
{
	bs->Read(m_x);
	bs->Read(m_z);
}
