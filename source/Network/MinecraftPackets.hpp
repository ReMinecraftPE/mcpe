#pragma once

#include "Packet.hpp"

class MinecraftPackets
{
public:
	static Packet* createPacket(int type);
};

