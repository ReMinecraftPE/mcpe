#pragma once

#include "thirdparty/raknet/BitStream.h"
#include "world/entity/Entity.hpp"

class PacketUtil
{
public:
	static char Rot_degreesToChar(float degrees);
	static float Rot_charToDegrees(char charValue);
	static void Rot_entityToChar(const Entity* entity, char& yawChar, char& pitchChar);
	static void Rot_charToEntity(Entity* entity, char yawChar, char pitchChar);

	static void WriteUserData(const ItemStack& item, RakNet::BitStream& bs, bool minData);
	static void ReadUserData(ItemStack& item, RakNet::BitStream& bs);
	static void WriteItemStack(const ItemStack& item, RakNet::BitStream& bs, bool doUserData, bool minUserData = false);
	static ItemStack ReadItemStack(RakNet::BitStream& bs, bool doUserData);
};

