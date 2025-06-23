/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "../Packet.hpp"

void RequestChunkPacket::handle(const RakNet::RakNetGUID& guid, NetEventCallback* pCallback)
{
	pCallback->handle(guid, this);
}

void RequestChunkPacket::write(RakNet::BitStream* bs)
{
	bs->Write((unsigned char)PACKET_REQUEST_CHUNK);
	bs->Write(m_chunkPos.x);
	bs->Write(m_chunkPos.z);
}

void RequestChunkPacket::read(RakNet::BitStream* bs)
{
	bs->Read(m_chunkPos.x);
	bs->Read(m_chunkPos.z);
}
