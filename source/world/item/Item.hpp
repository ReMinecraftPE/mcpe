/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include <string>
#include <stdint.h>
#include "common/Utils.hpp"
#include "world/level/Material.hpp"
#include "ItemInstance.hpp"
#include "world/level/TilePos.hpp"
#include "world/Facing.hpp"

#define C_MAX_ITEMS (C_MAX_TILES * 2)

class ItemInstance; // in case we're included from ItemInstance.hpp

class Level;
class Entity;
class Mob;
class Player;
class Tile;
class CompoundTag;

class Item
{
public: // Sub structures
	struct Tier
	{
		int   field_0;
		int   m_Durability;
		float m_HitStrength;
		int   field_C;

		Tier(int a, int b, float c, int d) :
			field_0(a),
			m_Durability(b),
			m_HitStrength(c),
			field_C(d)
		{}

		// Item tiers
		static Tier WOOD, STONE, IRON, DIAMOND, GOLD;
	};

public: // Methods
	Item(int ID);

	//@NOTE: The setters are virtual for whatever reason

	virtual Item* setIcon(int icon);
	virtual Item* setMaxStackSize(int mss);
	virtual Item* setIcon(int ix, int iy);
	virtual int getIcon(const ItemInstance*) const;
	virtual bool useOn(ItemInstance*, Level*, const TilePos& pos, Facing::Name face);
	virtual bool useOn(ItemInstance*, Player*, Level*, const TilePos& pos, Facing::Name face);
	virtual float getDestroySpeed(ItemInstance*, Tile*);
	virtual ItemInstance* use(ItemInstance*, Level*, Player*);
	virtual int getMaxStackSize();
	virtual int getLevelDataForAuxValue(int x);
	virtual bool isStackedByData();
	virtual int getMaxDamage();
	virtual void hurtEnemy(ItemInstance*, Mob*);
	virtual void mineBlock(ItemInstance*, const TilePos& pos, Facing::Name face);
	virtual int getAttackDamage(Entity*);
	virtual bool canDestroySpecial(Tile*);
	virtual void interactEnemy(ItemInstance*, Mob*);
	virtual Item* handEquipped();
	virtual bool isHandEquipped();
	virtual bool isMirroredArt();
	virtual Item* setDescriptionId(const std::string& desc);
	virtual std::string getDescription();
	virtual std::string getDescription(ItemInstance*);
	virtual std::string getDescriptionId();
	virtual std::string getDescriptionId(ItemInstance*);
	virtual Item* setCraftingRemainingItem(Item*);
	virtual Item* getCraftingRemainingItem();
	virtual bool hasCraftingRemainingItem();
	virtual std::string getName();
	virtual int buildIdAux(int16_t auxValue, const CompoundTag* userData = nullptr);

	static void initItems();
	
public: // Item class fields
	int m_itemID;
	int m_maxStackSize;
	int m_maxDamage;
	int m_icon;
	bool m_bHandEquipped;
	bool m_bStackedByData;
	Item* m_pCraftingRemainingItem;
	std::string m_DescriptionID;

public: // Static declarations
	static std::string ICON_DESCRIPTION_PREFIX;

	// The item array.
	static Item* items[C_MAX_ITEMS];

	// Common item definitions
	static Item
		*ironShovel,
		*ironPickaxe,
		*ironAxe,
		*flintAndSteel,
		*apple,
		*bow,
		*arrow,
		*coal,
		*diamond,
		*ironIngot,
		*goldIngot,
		*ironSword,
		*woodSword,
		*woodShovel,
		*woodPickaxe,
		*woodAxe,
		*stoneSword,
		*stoneShovel,
		*stonePickaxe,
		*stoneAxe,
		*diamondSword,
		*diamondShovel,
		*diamondPickaxe,
		*diamondAxe,
		*stick,
		*bowl,
		*mushroomStew,
		*goldSword,
		*goldShovel,
		*goldPickaxe,
		*goldAxe,
		*string,
		*feather,
		*sulphur,
		*woodHoe,
		*stoneHoe,
		*ironHoe,
		*diamondHoe,
		*goldHoe,
		*seeds,
		*wheat,
		*bread,
		*clothHelmet,
		*clothChestplate,
		*clothLeggings,
		*clothBoots,
		*chainHelmet,
		*chainChestplate,
		*chainLeggings,
		*chainBoots,
		*ironHelmet,
		*ironChestplate,
		*ironLeggings,
		*ironBoots,
		*diamondHelmet,
		*diamondChestplate,
		*diamondLeggings,
		*diamondBoots,
		*goldHelmet,
		*goldChestplate,
		*goldLeggings,
		*goldBoots,
		*flint,
		*rawPorkchop,
		*cookedPorkchop,
		*painting,
		*goldApple,
		*sign,
		*woodDoor,
		*emptyBucket,
		*waterBucket,
		*lavaBucket,
		*minecart,
		*saddle,
		*ironDoor,
		*redStone,
		*snowBall,
		*boat,
		*leather,
		*milk,
		*brick,
		*clay,
		*reeds,
		*paper,
		*book,
		*slimeBall,
		*chestMinecart,
		*furnaceMinecart,
		*egg,
		*compass,
		*fishingRod,
		*clock,
		*yellowDust,
		*rawFish,
		*cookedFish,
		*dyePowder,
		*bone,
		*sugar,
		*cake,
		*bed,
		*diode,
		*cookie,
		*map,
		*shears,
		*record_01,
		*record_02,
		*quiver,
		*camera,
		*rocket;
};
