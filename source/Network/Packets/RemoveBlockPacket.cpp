#include "Packet.hpp"

void RemoveBlockPacket::handle(const RakNet::RakNetGUID& guid, NetEventCallback* pCallback)
{
	pCallback->handle(guid, this);
}

void RemoveBlockPacket::write(RakNet::BitStream* bs)
{
	bs->Write(PACKET_REMOVE_BLOCK);
	bs->Write(m_playerID);
	bs->Write(m_x);
	bs->Write(m_z);
	bs->Write(m_y);
}

void RemoveBlockPacket::read(RakNet::BitStream* bs)
{
	bs->Read(m_playerID);
	bs->Read(m_x);
	bs->Read(m_z);
	bs->Read(m_y);
}
