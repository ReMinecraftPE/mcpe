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

	static void WriteUserData(const ItemInstance& item, RakNet::BitStream* bs, bool minData);
	static void ReadUserData(ItemInstance& item, RakNet::BitStream* bs);
	static void WriteItemInstance(const ItemInstance& item, RakNet::BitStream* bs, bool doUserData, bool minUserData = false);
	static ItemInstance ReadItemInstance(RakNet::BitStream* bs, bool doUserData);
};

