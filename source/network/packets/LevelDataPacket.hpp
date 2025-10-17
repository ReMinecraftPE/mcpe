#pragma once

#include "../Packet.hpp"

class LevelDataPacket : public Packet
{
public:
	LevelDataPacket()
	{
		m_pLevel = nullptr;
	}
	LevelDataPacket(Level* level) : m_pLevel(level) {}
	void handle(const RakNet::RakNetGUID&, NetEventCallback& callback) override;
	void write(RakNet::BitStream&) override;
	void read(RakNet::BitStream&) override;
public:
	RakNet::BitStream m_data;
	Level* m_pLevel;
};