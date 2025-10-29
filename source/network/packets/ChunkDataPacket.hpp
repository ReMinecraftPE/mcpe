#pragma once

#include "../Packet.hpp"
#include "world/level/levelgen/chunk/ChunkPos.hpp"

class ChunkDataPacket : public Packet
{
public:
	ChunkDataPacket()
	{
		m_pChunk = nullptr;
	}
	ChunkDataPacket(const ChunkPos& pos, LevelChunk* c) :m_chunkPos(pos), m_pChunk(c) {}
	void handle(const RakNet::RakNetGUID&, NetEventCallback& callback) override;
	void write(RakNet::BitStream&) override;
	void read(RakNet::BitStream&) override;
public:
	ChunkPos m_chunkPos;
	RakNet::BitStream m_data;
	LevelChunk* m_pChunk;
};