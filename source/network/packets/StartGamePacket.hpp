#pragma once

#include "../Packet.hpp"
#include "world/gamemode/GameType.hpp"
#include "world/phys/Vec3.hpp"

class StartGamePacket : public Packet
{
public:
	StartGamePacket()
	{
		m_seed = 0;
		m_levelVersion = 0;
		m_gameType = GAME_TYPES_MAX;
		m_entityId = 0;
		m_time = 0;
	}
	void handle(const RakNet::RakNetGUID&, NetEventCallback& callback) override;
	void write(RakNet::BitStream&) override;
	void read(RakNet::BitStream&) override;
public:
	int32_t m_seed;
	int m_levelVersion;
	GameType m_gameType;
	int m_entityId;
	int m_time;
	Vec3 m_pos;
};