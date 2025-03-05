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
	// The order of the TilePos matters here
	bs->Write(m_pos.x);
	bs->Write(m_pos.z);
	bs->Write(m_pos.y);
	bs->Write(m_face);
	bs->Write(m_tile);
}

void PlaceBlockPacket::read(RakNet::BitStream* bs)
{
	bs->Read(m_playerID);
	// The order of the TilePos matters here
	bs->Read(m_pos.x);
	bs->Read(m_pos.z);
	bs->Read(m_pos.y);
	bs->Read(m_face);
	bs->Read(m_tile);
}
