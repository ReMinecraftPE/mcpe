#include "Packet.hpp"

void RemoveEntityPacket::handle(const RakNet::RakNetGUID& guid, NetEventCallback* pCallback)
{
	pCallback->handle(guid, this);
}

void RemoveEntityPacket::write(RakNet::BitStream* bs)
{
	bs->Write(PACKET_REMOVE_ENTITY);
	bs->Write(m_id);
}

void RemoveEntityPacket::read(RakNet::BitStream* bs)
{
	bs->Read(m_id);
}
