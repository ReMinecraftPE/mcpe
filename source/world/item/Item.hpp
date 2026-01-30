/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include <string>
#include <stdint.h>

// needed for TileData and Tile IDs
#include "common/Utils.hpp"

#include "world/level/Material.hpp"
#include "world/level/TilePos.hpp"
#include "world/Facing.hpp"

#include "ItemStack.hpp"

#define C_MAX_ITEMS (C_MAX_TILES * 2)

class ItemStack; // in case we're included from ItemStack.hpp

class Level;
class Entity;
class Mob;
class Player;
class Tile;
class CompoundTag;

class Item
{
public: // Sub structures
	enum EquipmentSlot
	{
		SLOT_NONE = -1,
		SLOT_FEET,
		SLOT_LEGS,
		SLOT_CHEST,
		SLOT_HEAD
	};

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
	Item(int ID = TILE_AIR);

	//@NOTE: The setters are virtual for whatever reason

	virtual Item* setIcon(int icon);
	virtual Item* setMaxStackSize(int mss);
	virtual Item* setIcon(int ix, int iy);
	virtual int getIcon(const ItemStack*) const;
	virtual bool useOn(ItemStack*, Level*, const TilePos& pos, Facing::Name face) const;
	virtual bool useOn(ItemStack*, Player*, Level*, const TilePos& pos, Facing::Name face) const;
	virtual float getDestroySpeed(ItemStack*, Tile*) const;
	virtual ItemStack* use(ItemStack*, Level*, Player*) const;
	virtual int getMaxStackSize() const;
	virtual TileData getLevelDataForAuxValue(int x) const;
	virtual bool isStackedByData() const;
	virtual int getMaxDamage() const;
	virtual void hurtEnemy(ItemStack*, Mob*) const;
	virtual void mineBlock(ItemStack*, const TilePos& pos, Facing::Name face) const;
	virtual int getAttackDamage(Entity*) const;
	virtual bool canDestroySpecial(Tile*) const;
	virtual void interactEnemy(ItemStack*, Mob*) const;
	virtual Item* handEquipped();
	virtual bool isHandEquipped() const;
	virtual bool isMirroredArt() const;
	virtual Item* setDescriptionId(const std::string& desc);
	virtual std::string getDescription() const;
	virtual std::string getDescription(ItemStack*) const;
	virtual std::string getDescriptionId() const;
	virtual std::string getDescriptionId(ItemStack*) const;
	virtual Item* setCraftingRemainingItem(Item*);
	virtual Item* getCraftingRemainingItem() const;
	virtual bool hasCraftingRemainingItem() const;
	virtual std::string getName() const;
	virtual std::string getHovertextName() const;
	virtual void onCraftedBy(ItemStack*, Player*, Level*);
	virtual void inventoryTick(ItemStack*, Level*, Entity*, int, bool);
	virtual bool isDamageable() const;
	virtual int buildIdAux(int16_t auxValue, const CompoundTag* userData = nullptr) const;

	// Custom methods
	virtual EquipmentSlot getEquipmentSlot() const;
	virtual int getDefense() const;

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
	*camera,
	*rocket,
	*quiver;
};
