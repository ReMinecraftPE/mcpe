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

#include "ItemInstance.hpp"

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
		static Tier WOOD, STONE, IRON, EMERALD, GOLD;
	};

public: // Methods
	Item(int ID = TILE_AIR);

	//@NOTE: The setters are virtual for whatever reason

	virtual Item* setIcon(int icon);
	virtual Item* setMaxStackSize(int mss);
	virtual Item* setIcon(int ix, int iy);
	virtual int getIcon(const ItemInstance*) const;
	virtual bool useOn(ItemInstance*, Level*, const TilePos& pos, Facing::Name face) const;
	virtual bool useOn(ItemInstance*, Player*, Level*, const TilePos& pos, Facing::Name face) const;
	virtual float getDestroySpeed(ItemInstance*, Tile*) const;
	virtual ItemInstance* use(ItemInstance*, Level*, Player*) const;
	virtual int getMaxStackSize() const;
	virtual TileData getLevelDataForAuxValue(int x) const;
	virtual bool isStackedByData() const;
	virtual int getMaxDamage() const;
	virtual void hurtEnemy(ItemInstance*, Mob*) const;
	virtual void mineBlock(ItemInstance*, const TilePos& pos, Facing::Name face) const;
	virtual int getAttackDamage(Entity*) const;
	virtual bool canDestroySpecial(Tile*) const;
	virtual void interactEnemy(ItemInstance*, Mob*) const;
	virtual Item* handEquipped();
	virtual bool isHandEquipped() const;
	virtual bool isMirroredArt() const;
	virtual Item* setDescriptionId(const std::string& desc);
	virtual std::string getDescription() const;
	virtual std::string getDescription(ItemInstance*) const;
	virtual std::string getDescriptionId() const;
	virtual std::string getDescriptionId(ItemInstance*) const;
	virtual Item* setCraftingRemainingItem(Item*);
	virtual Item* getCraftingRemainingItem() const;
	virtual bool hasCraftingRemainingItem() const;
	virtual std::string getName() const;
	virtual std::string getHovertextName() const;
	virtual int buildIdAux(int16_t auxValue, const CompoundTag* userData = nullptr) const;

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
		*shovel_iron,
		*pickAxe_iron,
		*hatchet_iron,
		*flintAndSteel,
		*apple,
		*bow,
		*arrow,
		*coal,
		*emerald,
		*ironIngot,
		*goldIngot,
		*sword_iron,
		*sword_wood,
		*shovel_wood,
		*pickAxe_wood,
		*hatchet_wood,
		*sword_stone,
		*shovel_stone,
		*pickAxe_stone,
		*hatchet_stone,
		*sword_emerald,
		*shovel_emerald,
		*pickAxe_emerald,
		*hatchet_emerald,
		*stick,
		*bowl,
		*mushroomStew,
		*sword_gold,
		*shovel_gold,
		*pickAxe_gold,
		*hatchet_gold,
		*string,
		*feather,
		*sulphur,
		*hoe_wood,
		*hoe_stone,
		*hoe_iron,
		*hoe_emerald,
		*hoe_gold,
		*seeds,
		*wheat,
		*bread,
		*helmet_cloth,
		*chestplate_cloth,
		*leggings_cloth,
		*boots_cloth,
		*helmet_iron,
		*chestplate_iron,
		*leggings_iron,
		*boots_iron,
		*helmet_diamond,
		*chestplate_diamond,
		*leggings_diamond,
		*boots_diamond,
		*helmet_gold,
		*chestplate_gold,
		*leggings_gold,
		*boots_gold,
		*flint,
		*porkChop_raw,
		*porkChop_cooked,
		*painting,
		*apple_gold,
		*sign,
		*door_wood,
		*bucket_empty,
		*bucket_water,
		*bucket_lava,
		*minecart,
		*saddle,
		*door_iron,
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
		*minecart_chest,
		*minecart_furnace,
		*egg,
		*compass,
		*fishingRod,
		*clock,
		*yellowDust,
		*fish_raw,
		*fish_cooked,
		*dye_powder,
		*bone,
		*sugar,
		*cake,
		*bed,
		*diode,
		*cookie,
		*record_01,
		*record_02,
		*camera,
		*rocket,
		*quiver;
};
