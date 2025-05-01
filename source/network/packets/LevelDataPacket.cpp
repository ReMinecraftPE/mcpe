/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "../Packet.hpp"
#include "world/level/levelgen/chunk/LevelChunk.hpp"

void LevelDataPacket::handle(const RakNet::RakNetGUID& guid, NetEventCallback* pCallback)
{
	pCallback->handle(guid, this);
}

void LevelDataPacket::write(RakNet::BitStream* pbs)
{
	// @TODO: Maybe offload this to a different 'worker thread'? Or maybe just the compression job?

	// send a crapton of them in a raw packet(why? Laziness)
	int chunksX = C_MAX_CHUNKS_X;
	int chunksZ = C_MAX_CHUNKS_Z;
	int minus9999 = -9999;
	RakNet::BitStream bs;
	pbs->Write((unsigned char)PACKET_LEVEL_DATA);

	int uncompMagic = 12847812, compMagic = 58712758, chunkSepMagic = 284787658;
	bs.Write(uncompMagic);
	bs.Write(chunksX);
	bs.Write(chunksZ);
	ChunkPos chunkPos(0, 0);
	for (chunkPos.x = 0; chunkPos.x < chunksX; chunkPos.x++)
	{
		for (chunkPos.z = 0; chunkPos.z < chunksZ; chunkPos.z++)
		{
			bs.Write(chunkSepMagic);

			RakNet::BitStream bs2;
			LevelChunk* pChunk = m_pLevel->getChunk(chunkPos);
			ChunkDataPacket cdp(chunkPos, pChunk);
			cdp.write(&bs2);

			int dataSize = int(bs2.GetNumberOfBytesUsed());
			bs.Write(dataSize);
			bs.Write((const char*)bs2.GetData(), dataSize);
		}
	}

	// compress it
	size_t compSize = 0;
	size_t uncompSize = bs.GetNumberOfBytesUsed();
	uint8_t* pCompressedData = nullptr;

	if (uncompSize > 1024)
	{
		// takes about 5ms on release to compress everything on L4, compression ratio is around 14-15%.
		// Increase or decrease this depending on your server's load.
		pCompressedData = ZlibDeflateToMemoryLvl(bs.GetData(), uncompSize, &compSize, 4);
	}

	if (pCompressedData)
	{
		float ratio = 100.0f * float(compSize) / float(uncompSize);
		//LOG_I("Compression ratio: %.2f (%d comp, %d uncomp)", ratio, int(compSize), int(uncompSize));

		int cs2 = int(compSize), us2 = int(uncompSize);
		bs.Reset();
		bs.Write(compMagic);
		bs.Write(us2);
		bs.Write(cs2);
		bs.Write((const char*)pCompressedData, compSize);
		SAFE_DELETE_ARRAY(pCompressedData);
	}
	else
	{
		//LOG_I("Level not compressed.");
	}

	pbs->Write(bs);
	return;
}

void LevelDataPacket::read(RakNet::BitStream* bs)
{
	bs->Read(m_data);
}
