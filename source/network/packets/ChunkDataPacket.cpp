/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "../Packet.hpp"
#include "world/level/levelgen/chunk/LevelChunk.hpp"

void ChunkDataPacket::handle(const RakNet::RakNetGUID& guid, NetEventCallback* pCallback)
{
	pCallback->handle(guid, this);
}

void ChunkDataPacket::write(RakNet::BitStream* bs)
{
	bs->Write((unsigned char)PACKET_CHUNK_DATA);
	bs->Write(m_chunkPos.x);
	bs->Write(m_chunkPos.z);
	
	// Well, we first have to prepare the data.
	m_data.Reset();

	for (int i = 0; i < 256; i++)
	{
		m_data.Write(m_pChunk->m_updateMap[i]);

		// if nothing was updated:
		if (!m_pChunk->m_updateMap[i])
			continue;

		for (int y = 0; y < 8; y++)
		{
			if ((m_pChunk->m_updateMap[i] >> y) & 1)
			{
				int idx = ((i & 0xF) << 11) | ((i >> 4) << 7) + (y * 16);
				//write the tile data
				
				m_data.Write((const char*) &m_pChunk->m_pBlockData[idx],      16 * sizeof(TileID));
				m_data.Write((const char*) &m_pChunk->m_tileData  [idx >> 1], 8);
			}
		}
	}

	m_data.ResetReadPointer();
	bs->Write(m_data);
}

void ChunkDataPacket::read(RakNet::BitStream* bs)
{
	bs->Read(m_chunkPos.x);
	bs->Read(m_chunkPos.z);
	bs->Read(m_data);
	m_data.ResetReadPointer();
}
