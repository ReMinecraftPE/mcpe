/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include <ctime>
#include <cstdio> // have to include this to avoid it being included again later from being a problem
#include <cstdint>
#include <cassert>

#ifdef _WIN32

 // @HACK: Include WinSock2.h also
#include <WinSock2.h>
#include <Windows.h>
#include <WS2tcpip.h>

#endif

#include "compat/AKeyCodes.hpp"

#ifndef ORIGINAL_CODE
// Enhancements
#define ENH_ENTITY_SHADING      // Allows shading of entities
#define ENH_SHADE_HELD_TILES    // Allows shading of the item in hand
#define ENH_FIX_INVIS_STAIRS    // Fixes a bug wherein a 16x16x16 chunk in the world that contains only stairs is invisible
#define ENH_ALLOW_AO            // Allows using the F4 key to toggle ambient occlusion (buggy)
#define ENH_TRANSPARENT_HOTBAR  // Allows the hotbar to be transparent. Due to a bug in the code, it is not.
#define ENH_INSTA_BREAK         // Allows instant breaking of blocks. @TODO: Fix the mode without this
#define ENH_CAMERA_NO_PARTICLES // Hide particles from the view of a camera, such as smoke, that would otherwise render the resulting image useless.
#define ENH_USE_JAVA_LIGHT_RAMP // Use Java Beta 1.3 light ramp instead of flawed PE one
#define ENH_RUN_DAY_NIGHT_CYCLE // Allow the day/night cycle to run.
#define ENH_ENABLE_9TH_SLOT     // Enable the 9th hotbar slot, instead of it being a "..." placeholder
#define ENH_USE_OWN_AO          // Use own ambient occlusion engine - looks pretty much the same except it fixes the corners
#define ENH_ADD_OPTIONS_PAUSE   // Add an 'options' button in the pause menu
#define ENH_EXTRA_ITEMS_IN_INV  // Add extra items in a new 5th row in the inventory.
#define ENH_HIGHLIGHT_BY_HOVER  // Highlight buttons by hovering them instead of the usual way.
#define ENH_ALLOW_SAND_GRAVITY  // Allow sand to fall.
//#define ENH_USE_GUI_SCALE_2     // Use a 2x GUI scale instead of 3x. Looks better on PC

// Mods
//#define MOD_USE_FLAT_WORLD         // Use a flat world instead of the regular world generation
#define MOD_USE_BIGGER_SCREEN_SIZE // Use a bigger screen size instead of 854x480
//#define MOD_DONT_COLOR_GRASS       // Don't give the top of grass tiles a different color. (like Classic)

// Tests
//#define TEST_DROPPED_ITEMS      // Allow dropped items to be dropped and collected.

// Toggle Demo Mode
//#define DEMO

#else

#define DEMO

#endif

// don't know where to declare these:

#ifndef MOD_USE_BIGGER_SCREEN_SIZE
#define C_DEFAULT_SCREEN_WIDTH  (854)
#define C_DEFAULT_SCREEN_HEIGHT (480)
#else
#define C_DEFAULT_SCREEN_WIDTH  (1280)
#define C_DEFAULT_SCREEN_HEIGHT (720)
#endif

#define C_MAX_TILES (256)

#define C_DEFAULT_PORT (19132)
#define C_MAX_CONNECTIONS (4) // pitiful

constexpr int C_MIN_X = -32000000, C_MAX_X = 32000000;
constexpr int C_MIN_Z = -32000000, C_MAX_Z = 32000000;
constexpr int C_MIN_Y = 0, C_MAX_Y = 128;

const char* GetTerrainName();
const char* GetItemsName();
const char* GetGUIBlocksName();

#ifdef ORIGINAL_CODE
#define C_TERRAIN_NAME "terrain.png"
#define C_ITEMS_NAME   "gui/items.png"
#define C_BLOCKS_NAME  "gui/gui_blocks.png"
#else
#define C_TERRAIN_NAME GetTerrainName()
#define C_ITEMS_NAME   "gui/items.png"
#define C_BLOCKS_NAME  "gui/gui_blocks.png"
#endif

#define C_MAX_CHUNKS_X (16)
#define C_MAX_CHUNKS_Z (16)

// 9 chunks around a player things will tick
#define C_TICK_DISTANCE_CHKS (9)

enum eTileID
{
	TILE_AIR,
	TILE_STONE,
	TILE_GRASS,
	TILE_DIRT,
	TILE_STONEBRICK,
	TILE_WOOD,

	TILE_BEDROCK = 7,

	TILE_WATER = 8,
	TILE_WATER_CALM,
	TILE_LAVA,
	TILE_LAVA_CALM,

	TILE_SAND = 12,
	TILE_GRAVEL,
	TILE_ORE_GOLD,
	TILE_ORE_IRON,
	TILE_ORE_COAL,

	TILE_TREE_TRUNK = 17,
	TILE_LEAVES,

	TILE_GLASS = 20,
	TILE_ORE_LAPIS,

	TILE_SANDSTONE = 24,

	TILE_CLOTH = 35,

	TILE_FLOWER = 37,
	TILE_ROSE,
	TILE_MUSHROOM_1,
	TILE_MUSHROOM_2,

	TILE_BLOCK_GOLD = 41,
	TILE_BLOCK_IRON,
	TILE_STONESLAB_FULL,
	TILE_STONESLAB_HALF,
	TILE_BRICKS,
	TILE_TNT,

	TILE_OBSIDIAN = 49,
	TILE_TORCH,
	TILE_FIRE,

	TILE_STAIRS_WOOD = 53,

	TILE_ORE_EMERALD = 56,
	TILE_BLOCK_EMERALD,

	TILE_FARMLAND = 60,

	TILE_DOOR_WOOD = 64,
	TILE_LADDER,

	TILE_STAIRS_STONE = 67,

	TILE_DOOR_IRON = 71,

	TILE_ORE_REDSTONE = 73,
	TILE_ORE_REDSTONE_LIT,

	TILE_TOPSNOW = 78,
	TILE_ICE,

	TILE_CLAY = 82,
	TILE_REEDS,

	TILE_INVISIBLE = 95,

	TILE_CLOTH_00 = 101,
	TILE_CLOTH_10,
	TILE_CLOTH_20,
	TILE_CLOTH_30,
	TILE_CLOTH_40,
	TILE_CLOTH_50,
	TILE_CLOTH_60,
	TILE_CLOTH_70,
	TILE_CLOTH_01,
	TILE_CLOTH_11,
	TILE_CLOTH_21,
	TILE_CLOTH_31,
	TILE_CLOTH_41,
	TILE_CLOTH_51,
	TILE_CLOTH_61,

	ITEM_SHOVEL_IRON = 256,
	ITEM_PICKAXE_IRON,
	ITEM_HATCHET_IRON,
	ITEM_FLINT_AND_STEEL,
	ITEM_APPLE,
	ITEM_BOW,
	ITEM_ARROW,
	ITEM_COAL,
	ITEM_EMERALD,
	ITEM_INGOT_IRON,
	ITEM_INGOT_GOLD,
	ITEM_SWORD_IRON,
	ITEM_SWORD_WOOD,
	ITEM_SHOVEL_WOOD,
	ITEM_PICKAXE_WOOD,
	ITEM_HATCHET_WOOD,
	ITEM_SWORD_STONE,
	ITEM_SHOVEL_STONE,
	ITEM_PICKAXE_STONE,
	ITEM_HATCHET_STONE,
	ITEM_SWORD_EMERALD,
	ITEM_SHOVEL_EMERALD,
	ITEM_PICKAXE_EMERALD,
	ITEM_HATCHET_EMERALD,
	ITEM_STICK,
	ITEM_BOWL,
	ITEM_STEW_MUSHROOM,
	ITEM_SWORD_GOLD,
	ITEM_SHOVEL_GOLD,
	ITEM_PICKAXE_GOLD,
	ITEM_HATCHET_GOLD,
	ITEM_STRING,
	ITEM_FEATHER,
	ITEM_SULPHUR,
	ITEM_HOE_WOOD,
	ITEM_HOE_STONE,
	ITEM_HOE_IRON,
	ITEM_HOE_EMERALD,
	ITEM_HOE_GOLD,
	ITEM_SEEDS,
	ITEM_WHEAT,
	ITEM_BREAD,
	ITEM_HELMET_CLOTH,
	ITEM_CHESTPLATE_CLOTH,
	ITEM_LEGGINGS_CLOTH,
	ITEM_BOOTS_CLOTH,
	ITEM_HELMET_CHAIN,
	ITEM_CHESTPLATE_CHAIN,
	ITEM_LEGGINGS_CHAIN,
	ITEM_BOOTS_CHAIN,
	ITEM_HELMET_IRON,
	ITEM_CHESTPLATE_IRON,
	ITEM_LEGGINGS_IRON,
	ITEM_BOOTS_IRON,
	ITEM_HELMET_EMERALD,
	ITEM_CHESTPLATE_EMERALD,
	ITEM_LEGGINGS_EMERALD,
	ITEM_BOOTS_EMERALD,
	ITEM_HELMET_GOLD,
	ITEM_CHESTPLATE_GOLD,
	ITEM_LEGGINGS_GOLD,
	ITEM_BOOTS_GOLD,
	ITEM_FLINT,
	ITEM_PORKCHOP_RAW,
	ITEM_PORKCHOP_COOKED,
	ITEM_PAINTING,
	ITEM_APPLE_GOLD,
	ITEM_SIGN,
	ITEM_DOOR_WOOD,
	ITEM_BUCKET,
	ITEM_BUCKET_WATER,
	ITEM_BUCKET_LAVA,
	ITEM_MINECART,
	ITEM_SADDLE,
	ITEM_DOOR_IRON,
	ITEM_REDSTONE,
	ITEM_SNOWBALL,
	ITEM_BOAT,
	ITEM_LEATHER,
	ITEM_BUCKET_MILK,
	ITEM_BRICK,
	ITEM_CLAY,
	ITEM_REEDS,
	ITEM_PAPER,
	ITEM_BOOK,
	ITEM_SLIME_BALL,
	ITEM_MINECART_CHEST,
	ITEM_MINECART_FURNACE,
	ITEM_EGG,
	ITEM_COMPASS,
	ITEM_FISHING_ROD,
	ITEM_CLOCK,
	ITEM_YELLOW_DUST,
	ITEM_FISH_RAW,
	ITEM_FISH_COOKED,
	ITEM_DYE_POWDER,
	ITEM_BONE,
	ITEM_SUGAR,
	ITEM_CAKE,
	ITEM_BED,
	ITEM_DIODE,
	ITEM_COOKIE,
	ITEM_RECORD_01,
	ITEM_RECORD_02,
	ITEM_CAMERA = 456,
};

enum // Textures
{
	TEXTURE_GRASS_TOP = 0,
	TEXTURE_STONE,
	TEXTURE_DIRT,
	TEXTURE_GRASS_SIDE,
	TEXTURE_PLANKS,
	TEXTURE_STONE_SLAB_SIDE,
	TEXTURE_STONE_SLAB_TOP,
	TEXTURE_BRICKS,
	TEXTURE_TNT_SIDE,
	TEXTURE_TNT_TOP,
	TEXTURE_TNT_BOTTOM,
	TEXTURE_COBWEB,
	TEXTURE_ROSE,
	TEXTURE_FLOWER,
	TEXTURE_WATER_STATIC,
	TEXTURE_SAPLING,
	TEXTURE_STONEBRICK,
	TEXTURE_BEDROCK,
	TEXTURE_SAND,
	TEXTURE_GRAVEL,
	TEXTURE_LOG_SIDE,
	TEXTURE_LOG_TOP,
	TEXTURE_IRON,
	TEXTURE_GOLD,
	TEXTURE_EMERALD,
	TEXTURE_CHEST_ONE_TOP,
	TEXTURE_CHEST_ONE_SIDE,
	TEXTURE_CHEST_ONE_FRONT,
	TEXTURE_MUSHROOM_RED,
	TEXTURE_MUSHROOM_BROWN,
	TEXTURE_NONE30,
	TEXTURE_FIRE1,
	TEXTURE_ORE_GOLD,
	TEXTURE_ORE_IRON,
	TEXTURE_ORE_COAL,
	TEXTURE_BOOKSHELF,
	TEXTURE_MOSSY_STONE,
	TEXTURE_OBSIDIAN,
	TEXTURE_OBSIDIAN_CRYING,
	TEXTURE_NONE39,
	TEXTURE_NONE40,
	TEXTURE_CHEST_TWO_FRONT_LEFT,
	TEXTURE_CHEST_TWO_FRONT_RIGHT,
	TEXTURE_WORKBENCH_TOP,
	TEXTURE_FURNACE_FRONT,
	TEXTURE_FURNACE_SIDE,
	TEXTURE_DISPENSER_SIDE,
	TEXTURE_FIRE2,
	TEXTURE_SPONGE,
	TEXTURE_GLASS,
	TEXTURE_ORE_EMERALD,
	TEXTURE_ORE_RED_STONE,
	TEXTURE_LEAVES_TRANSPARENT,
	TEXTURE_LEAVES_OPAQUE,
	TEXTURE_NONE54,
	TEXTURE_NONE55,
	TEXTURE_NONE56,
	TEXTURE_CHEST_TWO_BACK_LEFT,
	TEXTURE_CHEST_TWO_BACK_RIGHT,
	TEXTURE_WORKBENCH_SIDE_1,
	TEXTURE_WORKBENCH_SIDE_2,
	TEXTURE_FURNACE_LIT,
	TEXTURE_FURNACE_TOP,
	TEXTURE_NONE63,
	TEXTURE_CLOTH_64,
	TEXTURE_SPAWNER,
	TEXTURE_SNOW,
	TEXTURE_ICE,
	TEXTURE_GRASS_SIDE_SNOW,
	TEXTURE_CACTUS_TOP,
	TEXTURE_CACTUS_SIDE,
	TEXTURE_CACTUS_BOTTOM,
	TEXTURE_CLAY,
	TEXTURE_REEDS,
	TEXTURE_JUKEBOX_SIDE,
	TEXTURE_JUKEBOX_TOP,
	TEXTURE_NONE76,
	TEXTURE_NONE77,
	TEXTURE_NONE78,
	TEXTURE_NONE79,
	TEXTURE_TORCH_LIT,
	TEXTURE_DOOR_TOP,
	TEXTURE_DOOR_IRON_TOP,
	TEXTURE_LADDER,
	TEXTURE_NONE84,
	TEXTURE_NONE85,
	TEXTURE_FARMLAND,
	TEXTURE_FARMLAND_DRY,
	TEXTURE_WHEAT_0,
	TEXTURE_WHEAT_1,
	TEXTURE_WHEAT_2,
	TEXTURE_WHEAT_3,
	TEXTURE_WHEAT_4,
	TEXTURE_WHEAT_5,
	TEXTURE_WHEAT_6,
	TEXTURE_WHEAT_7,
	TEXTURE_LEVER,
	TEXTURE_DOOR_BOTTOM,
	TEXTURE_DOOR_IRON_BOTTOM,
	TEXTURE_TORCH_RED_STONE,
	TEXTURE_NONE100,
	TEXTURE_NONE101,
	TEXTURE_PUMPKIN_TOP,
	TEXTURE_BLOODSTONE,
	TEXTURE_SOULSAND,
	TEXTURE_GLOWSTONE,
	TEXTURE_NONE106,
	TEXTURE_NONE107,
	TEXTURE_NONE108,
	TEXTURE_NONE109,
	TEXTURE_NONE110,
	TEXTURE_NONE111,
	TEXTURE_RAIL_CURVED,
	TEXTURE_CLOTH_112,
	TEXTURE_CLOTH_113,
	TEXTURE_TORCH_RED_STONE_OFF,
	TEXTURE_LOG_SPRUCE,
	TEXTURE_LOG_BIRCH,
	TEXTURE_PUMPKIN_SIDE,
	TEXTURE_PUMPKIN_FACE,
	TEXTURE_PUMPKIN_FACE_LIT,
	TEXTURE_CAKE_TOP,
	TEXTURE_CAKE_SIDE,
	TEXTURE_CAKE_SIDE_BIT,
	TEXTURE_CAKE_BOTTOM,
	TEXTURE_NONE125,
	TEXTURE_NONE126,
	TEXTURE_NONE127,

	TEXTURE_ORE_LAPIS = 160,

	TEXTURE_SANDSTONE_TOP = 176,
	TEXTURE_SANDSTONE_SIDE = 192,
	TEXTURE_WATER = 205,
	TEXTURE_SANDSTONE_BOTTOM = 208,

	TEXTURE_LAVA = 237,

	TEXTURE_LAVA_PLACEHOLDER = 255,
};

enum eRenderShape
{
	SHAPE_NONE = -1,
	SHAPE_SOLID,
	SHAPE_CROSS,
	SHAPE_TORCH,
	SHAPE_FIRE,
	SHAPE_WATER,
	SHAPE_UNK5,
	SHAPE_UNK6,
	SHAPE_DOOR,
	SHAPE_LADDER,
	SHAPE_UNK9,
	SHAPE_STAIRS,
};

enum eRenderLayer
{
	LAYER_OPAQUE,
	LAYER_ALPHA
};

enum eDirection
{
	DIR_YNEG,
	DIR_YPOS,
	DIR_ZNEG, // North
	DIR_ZPOS, // South
	DIR_XNEG, // West
	DIR_XPOS, // East
};

struct ChunkPos
{
	int x = 0, z = 0;
	ChunkPos() {}
	ChunkPos(int _x, int _z) : x(_x), z(_z) {}

	bool operator<(const ChunkPos& b) const
	{
		if (x != b.x)
			return x < b.x;

		return z < b.z;
	}
};

struct Pos
{
	int x = 0, y = 0, z = 0;
	Pos() {}
	Pos(int _x, int _y, int _z) : x(_x), y(_y), z(_z) {}
};

// @NOTE: Duplicate?
struct TilePos
{
	int x = 0, y = 0, z = 0;
	TilePos() {}
	TilePos(int _x, int _y, int _z) : x(_x), y(_y), z(_z) {}

	bool operator<(const TilePos& b) const
	{
		if (x != b.x)
			return x < b.x;
		if (y != b.y)
			return y < b.y;

		return z < b.z;
	}
};

#define SAFE_DELETE(ptr) do { if (ptr) delete ptr; } while (0)
#define SAFE_DELETE_ARRAY(ptr) do { if (ptr) delete[] ptr; } while (0)

typedef uint8_t TileID;

// functions from Mojang
time_t getEpochTimeS();
time_t getRawTimeS();
float getTimeS();
int getTimeMs();

float Max(float a, float b);

void sleepMs(int ms);

// @NOTE: This is inlined.
constexpr float Lerp(float a, float b, float progress)
{
	return a + progress * (b - a);
}


// things that we added:
#ifndef ORIGINAL_CODE

void LogMsg(const char* fmt, ...);     // not part of actual Minecraft (they use printf), but useful
void LogMsgNoCR(const char* fmt, ...); // not part of actual Minecraft (they use printf), but useful

#ifdef printf
#error "printf is defined. Why?"
#endif

#define printf LogMsgNoCR
#define puts(str) LogMsg("%s", str);

#ifdef _WIN32

HINSTANCE GetInstance();
HWND GetHWND();
void CenterWindow(HWND hWnd);
void EnableOpenGL(HWND hwnd, HDC*, HGLRC*);
void DisableOpenGL(HWND, HDC, HGLRC);

void SetInstance(HINSTANCE hinst);
void SetHWND(HWND hwnd);

#endif

#else

#define LogMsg(...)
#define LogMsgNoCR(...)

#endif
