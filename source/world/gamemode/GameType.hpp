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
		static std::string survival = "Survival";
		static std::string creative = "Creative";
		static std::string adventure = "Adventure";
		static std::string spectator = "Spectator";
		switch (type)
		{
		case GAME_TYPE_SURVIVAL:
			return survival;
		case GAME_TYPE_CREATIVE:
			return creative;
		case GAME_TYPE_ADVENTURE:
			return adventure;
		case GAME_TYPE_SPECTATOR:
			return spectator;
		default:
			return Util::EMPTY_STRING;
		}
	}

	static const std::string& GameTypeDescriptionToNonLocString(GameType type)
	{
		static std::string survival = "Mobs, health and gather resources";
		static std::string creative = "Unlimited resources, flying";

		switch (type)
		{
		case GAME_TYPE_SURVIVAL:
			return survival;
		case GAME_TYPE_CREATIVE:
			return creative;
		case GAME_TYPE_ADVENTURE:
		case GAME_TYPE_SPECTATOR:
		default:
			return Util::EMPTY_STRING;
		}
	}
};
