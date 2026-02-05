#pragma once

#include "../Packet.hpp"
#include "world/level/TilePos.hpp"

class TileEventPacket : public Packet
{
public:
	TileEventPacket()
	{
		m_b0 = 0;
		m_b1 = 0;
	}
	TileEventPacket(const TilePos& pos, int32_t b0, int32_t b1);

	void handle(const RakNet::RakNetGUID&, NetEventCallback& callback) override;
	void write(RakNet::BitStream&) override;
	void read(RakNet::BitStream&) override;
public:
	TilePos m_pos;
	int32_t m_b0;
	int32_t m_b1;
};
