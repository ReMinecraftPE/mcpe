#pragma once

#include <stdint.h>

class LevelEvent
{
public:
	typedef int16_t ID;
	typedef int32_t Data;
	// https://minecraft.wiki/w/Bedrock_Edition_protocol/Level_Events
	enum Type
	{
		SOUND_CLICK                  = 1000,
		SOUND_CLICK_FAIL             = 1001,
		SOUND_SHOOT                  = 1002,
		SOUND_DOOR                   = 1003,
		SOUND_FIZZ                   = 1004,
		SOUND_IGNITE                 = 1005,
		SOUND_GHAST                  = 1007,
		SOUND_GHAST_SHOOT            = 1008,
		SOUND_BLAZE_SHOOT            = 1009,
		SOUND_DOOR_BUMP              = 1010,
		SOUND_DOOR_CRASH             = 1012,
		SOUND_ENDERMAN_TELEPORT      = 1018,
		SOUND_ANVIL_BREAK            = 1020,
		SOUND_ANVIL_USE              = 1021,
		SOUND_ANVIL_FALL             = 1022,
		SOUND_POP                    = 1030,
		SOUND_PORTAL                 = 1032,
		SOUND_ITEMFRAME_ADD_ITEM     = 1040,
		SOUND_ITEMFRAME_REMOVE       = 1041,
		SOUND_ITEMFRAME_PLACE        = 1042,
		SOUND_ITEMFRAME_REMOVE_ITEM  = 1043,
		SOUND_ITEMFRAME_ROTATE_ITEM  = 1044,
		SOUND_CAMERA                 = 1050,
		SOUND_ORB                    = 1051,
		SOUND_TOTEM                  = 1052,
		SOUND_ARMOR_STAND_BREAK      = 1060,
		SOUND_ARMOR_STAND_HIT        = 1061,
		SOUND_ARMOR_STAND_FALL       = 1062,
		SOUND_ARMOR_STAND_PLACE      = 1063,
		PARTICLE_SHOOT               = 2000,
		PARTICLE_DESTROY             = 2001,
		PARTICLE_SPLASH              = 2002,
		PARTICLE_EYE_DESPAWN         = 2003,
		PARTICLE_SPAWN               = 2004,
		ELDER_GUARDIAN_CURSE         = 2006,
		PARTICLE_BLOCK_FORCE_FIELD   = 2008,
		PARTICLE_PROJECTILE_HIT      = 2009,
		PARTICLE_DRAGON_EGG_TELEPORT = 2010,
		PARTICLE_ENDERMAN_TELEPORT   = 2013,
		PARTICLE_PUNCH_BLOCK         = 2014,
		START_RAIN                   = 3001,
		START_THUNDER                = 3002,
		STOP_RAIN                    = 3003,
		STOP_THUNDER                 = 3004,
		PAUSE_GAME                   = 3005, // Data: 1 to pause, 0 to resume
		PAUSE_GAME_NO_SCREEN         = 3006, // Data: 1 to pause, 0 to resume - same effect as normal pause but without screen
		SET_GAME_SPEED               = 3007, // X coordinate of pos = scale factor (default 1.0)
		REDSTONE_TRIGGER             = 3500,
		CAULDRON_EXPLODE             = 3501,
		CAULDRON_DYE_ARMOR           = 3502,
		CAULDRON_CLEAN_ARMOR         = 3503,
		CAULDRON_FILL_POTION         = 3504,
		CAULDRON_TAKE_POTION         = 3505,
		CAULDRON_FILL_WATER          = 3506,
		CAULDRON_TAKE_WATER          = 3507,
		CAULDRON_ADD_DYE             = 3508,
		CAULDRON_CLEAN_BANNER        = 3509,
		BLOCK_START_BREAK            = 3600,
		BLOCK_STOP_BREAK             = 3601,
		SET_DATA                     = 4000,
		PLAYERS_SLEEPING             = 9800
	};
};