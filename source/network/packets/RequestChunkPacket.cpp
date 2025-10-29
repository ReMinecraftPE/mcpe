/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "RequestChunkPacket.hpp"
#include "network/NetEventCallback.hpp"

void RequestChunkPacket::handle(const RakNet::RakNetGUID& guid, NetEventCallback& callback)
{
	callback.handle(guid, this);
}

void RequestChunkPacket::write(RakNet::BitStream& bs)
{
	bs.Write((unsigned char)PACKET_REQUEST_CHUNK);
	bs.Write(m_chunkPos.x);
	bs.Write(m_chunkPos.z);
}

void RequestChunkPacket::read(RakNet::BitStream& bs)
{
	bs.Read(m_chunkPos.x);
	bs.Read(m_chunkPos.z);
}
