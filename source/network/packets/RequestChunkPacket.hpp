#pragma once

#include "../Packet.hpp"
#include "world/level/levelgen/chunk/ChunkPos.hpp"

class RequestChunkPacket : public Packet
{
public:
	RequestChunkPacket() {}
	RequestChunkPacket(const ChunkPos& pos) { m_chunkPos = pos; }
	void handle(const RakNet::RakNetGUID&, NetEventCallback& callback) override;
	void write(RakNet::BitStream&) override;
	void read(RakNet::BitStream&) override;
public:
	ChunkPos m_chunkPos;
};