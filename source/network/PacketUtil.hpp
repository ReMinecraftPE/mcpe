#pragma once

#include "world/entity/Entity.hpp"

class PacketUtil
{
public:
	static char Rot_degreesToChar(float degrees);
	static float Rot_charToDegrees(char charValue);
	static void Rot_entityToChar(const Entity* entity, char& yawChar, char& pitchChar);
	static void Rot_charToEntity(Entity* entity, char yawChar, char pitchChar);
};

