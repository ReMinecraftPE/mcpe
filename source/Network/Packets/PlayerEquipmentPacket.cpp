#include "Packet.hpp"

void PlayerEquipmentPacket::handle(const RakNet::RakNetGUID& guid, NetEventCallback* pCallback)
{
	pCallback->handle(guid, this);
}

void PlayerEquipmentPacket::write(RakNet::BitStream* bs)
{
	bs->Write(PACKET_PLAYER_EQUIPMENT);
	bs->Write(m_playerID);
	bs->Write(m_itemID);
}

void PlayerEquipmentPacket::read(RakNet::BitStream* bs)
{
	bs->Read(m_playerID);
	bs->Read(m_itemID);
}
