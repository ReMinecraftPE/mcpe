#include "Packet.hpp"

void StartGamePacket::handle(const RakNet::RakNetGUID& guid, NetEventCallback* pCallback)
{
	pCallback->handle(guid, this);
}

void StartGamePacket::write(RakNet::BitStream* bs)
{
	bs->Write(PACKET_START_GAME);
	bs->Write(field_4);
	bs->Write(field_8);
	bs->Write(field_C);
	bs->Write(field_10);
	bs->Write(field_14);
	bs->Write(field_18);
}

void StartGamePacket::read(RakNet::BitStream* bs)
{
	bs->Read(field_4);
	bs->Read(field_8);
	bs->Read(field_C);
	bs->Read(field_10);
	bs->Read(field_14);
	bs->Read(field_18);
}