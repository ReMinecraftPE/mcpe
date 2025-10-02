#pragma once

#include "../Packet.hpp"
#include "common/Utils.hpp"
#include "world/level/TilePos.hpp"

class UpdateBlockPacket : public Packet
{
public:
	void handle(const RakNet::RakNetGUID&, NetEventCallback& callback) override;
	void write(RakNet::BitStream&) override;
	void read(RakNet::BitStream&) override;
public:
	TilePos m_pos;
	TileID m_tileTypeId;
	TileData m_data;
};