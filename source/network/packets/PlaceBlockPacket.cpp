/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "../Packet.hpp"

void PlaceBlockPacket::handle(const RakNet::RakNetGUID& guid, NetEventCallback* pCallback)
{
	pCallback->handle(guid, this);
}

void PlaceBlockPacket::write(RakNet::BitStream* bs)
{
	bs->Write((unsigned char)PACKET_PLACE_BLOCK);
	bs->Write(m_playerID);
	bs->Write(m_pos);
	bs->Write(m_face);
	bs->Write(m_tile);
}

void PlaceBlockPacket::read(RakNet::BitStream* bs)
{
	bs->Read(m_playerID);
	bs->Read(m_pos);
	bs->Read(m_face);
	bs->Read(m_tile);
}
