#pragma once

#include <string>

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
	static std::string GameTypeToNonLocString(GameType type)
	{
		switch (type)
		{
		case GAME_TYPE_SURVIVAL:
			return "Survival";
		case GAME_TYPE_CREATIVE:
			return "Creative";
		case GAME_TYPE_ADVENTURE:
			return "Adventure";
		case GAME_TYPE_SPECTATOR:
			return "Spectator";
		default:
			return "Undefined";
		}
	}
};