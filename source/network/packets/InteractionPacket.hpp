#pragma once

#include "../Packet.hpp"
#include "world/phys/Vec3.hpp"
#include "world/tile/Tile.hpp"

class InteractionPacket : public Packet
{
public:
	enum ActionType
	{
		SLEEP = 0
	};

public:
	InteractionPacket()
	{
		m_entityId = 0;
		m_actionType = SLEEP;
		m_pos = TilePos(0, 0, 0);
	}
	InteractionPacket(int32_t entityId, int8_t actionType, const TilePos& pos);

	void handle(const RakNet::RakNetGUID&, NetEventCallback& callback) override;
	void write(RakNet::BitStream&) override;
	void read(RakNet::BitStream&) override;
public:
	int32_t m_entityId;
	int8_t m_actionType;
	TilePos m_pos;
};
