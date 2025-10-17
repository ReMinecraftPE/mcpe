/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "LevelDataPacket.hpp"
#include "network/NetEventCallback.hpp"
#include "world/level/Level.hpp"
#include "world/level/levelgen/chunk/LevelChunk.hpp"
#include "ChunkDataPacket.hpp"

void LevelDataPacket::handle(const RakNet::RakNetGUID& guid, NetEventCallback& callback)
{
	callback.handle(guid, this);
}

void LevelDataPacket::write(RakNet::BitStream& bs)
{
	// @TODO: Maybe offload this to a different 'worker thread'? Or maybe just the compression job?

	// send a crapton of them in a raw packet(why? Laziness)
	int chunksX = C_MAX_CHUNKS_X;
	int chunksZ = C_MAX_CHUNKS_Z;
	int minus9999 = -9999;
	RakNet::BitStream bs2;
	bs.Write((unsigned char)PACKET_LEVEL_DATA);

	int uncompMagic = 12847812, compMagic = 58712758, chunkSepMagic = 284787658;
	bs2.Write(uncompMagic);
	bs2.Write(chunksX);
	bs2.Write(chunksZ);
	ChunkPos chunkPos(0, 0);
	for (chunkPos.x = 0; chunkPos.x < chunksX; chunkPos.x++)
	{
		for (chunkPos.z = 0; chunkPos.z < chunksZ; chunkPos.z++)
		{
			bs2.Write(chunkSepMagic);

			RakNet::BitStream bs3;
			LevelChunk* pChunk = m_pLevel->getChunk(chunkPos);
			ChunkDataPacket cdp(chunkPos, pChunk);
			cdp.write(bs3);

			int dataSize = int(bs3.GetNumberOfBytesUsed());
			bs2.Write(dataSize);
			bs2.Write((const char*)bs3.GetData(), dataSize);
		}
	}

	// compress it
	size_t compSize = 0;
	size_t uncompSize = bs2.GetNumberOfBytesUsed();
	uint8_t* pCompressedData = nullptr;

	if (uncompSize > 1024)
	{
		// takes about 5ms on release to compress everything on L4, compression ratio is around 14-15%.
		// Increase or decrease this depending on your server's load.
		pCompressedData = ZlibDeflateToMemoryLvl(bs2.GetData(), uncompSize, &compSize, 4);
	}

	if (pCompressedData)
	{
		float ratio = 100.0f * float(compSize) / float(uncompSize);
		//LOG_I("Compression ratio: %.2f (%d comp, %d uncomp)", ratio, int(compSize), int(uncompSize));

		int cs2 = int(compSize), us2 = int(uncompSize);
		bs2.Reset();
		bs2.Write(compMagic);
		bs2.Write(us2);
		bs2.Write(cs2);
		bs2.Write((const char*)pCompressedData, compSize);
		SAFE_DELETE_ARRAY(pCompressedData);
	}
	else
	{
		//LOG_I("Level not compressed.");
	}

	bs.Write(bs2);
	return;
}

void LevelDataPacket::read(RakNet::BitStream& bs)
{
	bs.Read(m_data);
}
