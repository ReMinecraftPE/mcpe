#pragma once

#include <string>
#include "common/Util.hpp"

enum GameType
{
	GAME_TYPE_SURVIVAL,
	GAME_TYPE_CREATIVE,
	GAME_TYPE_ADVENTURE,
	GAME_TYPE_SPECTATOR,
	GAME_TYPES_MIN = GAME_TYPE_SURVIVAL,
	GAME_TYPES_MAX = GAME_TYPE_CREATIVE
};

class GameTypeConv
{
public:
	static const std::string& GameTypeToNonLocString(GameType type)
	{
		switch (type)
		{
		case GAME_TYPE_SURVIVAL:
			static const std::string survival = "Survival";
			return survival;
		case GAME_TYPE_CREATIVE:
			static const std::string creative = "Creative";
			return creative;
		case GAME_TYPE_ADVENTURE:
			static const std::string adventure = "Adventure";
			return adventure;
		case GAME_TYPE_SPECTATOR:
			static const std::string spectator = "Spectator";
			return spectator;
		default:
			return Util::EMPTY_STRING;
		}
	}

	static const std::string& GameTypeDescriptionToNonLocString(GameType type)
	{
		switch (type)
		{
		case GAME_TYPE_SURVIVAL:
			static const std::string survival = "Mobs, health and gather resources";
			return survival;
		case GAME_TYPE_CREATIVE:
			static const std::string creative = "Unlimited resources, flying";
			return creative;
		case GAME_TYPE_ADVENTURE:
		case GAME_TYPE_SPECTATOR:
		default:
			return Util::EMPTY_STRING;
		}
	}
};
