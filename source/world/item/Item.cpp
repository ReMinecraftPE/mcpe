/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "Item.hpp"

#include "common/Logger.hpp"

#include "CameraItem.hpp"
#include "DoorItem.hpp"
#include "TileItem.hpp"
#include "TilePlanterItem.hpp"
#include "RocketItem.hpp"
//#include "BowlFoodItem.hpp"
//#include "SaddleItem.hpp"
//#include "BucketItem.hpp"
//#include "WeaponItem.hpp"
//#include "PickaxeItem.hpp"
//#include "AxeItem.hpp"
//#include "ShovelItem.hpp"
//#include "HoeItem.hpp"
//#include "ArmorItem.hpp"
//#include "FlintAndSteelItem.hpp"
//#include "CoalItem.hpp"
//#include "BowItem.hpp"
//#include "SeedItem.hpp"
//#include "MinecartItem.hpp"
//#include "BoatItem.hpp"
//#include "BedItem.hpp"
//#include "RecordItem.hpp"
//#include "SignItem.hpp"
//#include "EggItem.hpp"
//#include "PaintingItem.hpp"
//#include "FishingRodItem.hpp"
//#include "MapItem.hpp"
//#include "ShearsItem.hpp"

#define ITEM(x) ((x) - 256)

#define NEW_ITEM(id) (new Item(ITEM(id)))
#define NEW_X_ITEM(Type, id, arg) (new Type(ITEM(id), arg))
#define NEW_X_ITEMN(Type, id) (new Type(ITEM(id)))

static bool g_bInittedItems = false;

Item* Item::items[C_MAX_ITEMS];

Item::Item(int itemID)
{
	m_bHandEquipped = 0;
	m_itemID = itemID + 256;
	m_bStackedByData = 0;
	m_pCraftingRemainingItem = 0;
	m_maxStackSize = 64;
	m_maxDamage = 32;

#ifndef ORIGINAL_CODE
	//@UB: Not initializing m_icon.
	m_icon = 0;
#endif


	if (Item::items[m_itemID])
	{
		LOG_W("Item conflict id @ %d! Id already used\n", m_itemID);
	}

	Item::items[m_itemID] = this;
}

Item* Item::setIcon(int icon)
{
	m_icon = icon;
	return this;
}

Item* Item::setIcon(int ix, int iy)
{
	return setIcon(ix + 16 * iy);
}

Item* Item::setMaxStackSize(int mss)
{
	m_maxStackSize = mss;
	return this;
}

Item* Item::setCraftingRemainingItem(Item* pItem)
{
	if (m_maxStackSize > 1)
		LOG_W("Max stack size must be 1 for items with crafting results");

	m_pCraftingRemainingItem = pItem;

	return this;
}

Item* Item::setDescriptionId(const std::string& desc)
{
	m_DescriptionID = ICON_DESCRIPTION_PREFIX + desc;
	return this;
}

Item* Item::handEquipped()
{
	//@NOTE: This refers to the in-hand renderer when one is in 3rd person mode.
	// Instead of holding the item nearly parallel to the ground, the item is held
	// perpendicular to the ground. It can be seen in swords (where this is true)
	// vs. compasses (where this is false).
	m_bHandEquipped = true;
	return this;
}

Item::EquipmentSlot Item::getEquipmentSlot() const
{
	return SLOT_NONE;
}

int Item::getDefense() const
{
	return 0;
}

void Item::initItems()
{
	if (g_bInittedItems)
		return;

	g_bInittedItems = true;

	Item::bow = NEW_ITEM(ITEM_BOW)
		->setIcon(5, 1)
		->setDescriptionId("bow");

	Item::woodSword = NEW_ITEM(ITEM_SWORD_WOOD)
		->setIcon(0, 4)
		->setDescriptionId("swordWood")
		->handEquipped();

	Item::woodPickaxe = NEW_ITEM(ITEM_PICKAXE_WOOD)
		->setIcon(0, 6)
		->setDescriptionId("pickaxeWood")
		->handEquipped();

	Item::woodAxe = NEW_ITEM(ITEM_HATCHET_WOOD)
		->setIcon(0, 7)
		->setDescriptionId("hatchetWood")
		->handEquipped();

	Item::woodShovel = NEW_ITEM(ITEM_SHOVEL_WOOD)
		->setIcon(0, 5)
		->setDescriptionId("shovelWood")
		->handEquipped();

	Item::woodHoe = NEW_ITEM(ITEM_HOE_WOOD)
		->setIcon(0, 8)
		->setDescriptionId("hoeWood")
		->handEquipped();

	Item::stoneSword = NEW_ITEM(ITEM_SWORD_STONE)
		->setIcon(1, 4)
		->setDescriptionId("swordStone")
		->handEquipped();

	Item::stonePickaxe = NEW_ITEM(ITEM_PICKAXE_STONE)
		->setIcon(1, 6)
		->setDescriptionId("pickaxeStone")
		->handEquipped();

	Item::stoneAxe = NEW_ITEM(ITEM_HATCHET_STONE)
		->setIcon(1, 7)
		->setDescriptionId("hatchetStone")
		->handEquipped();

	Item::stoneShovel = NEW_ITEM(ITEM_SHOVEL_STONE)
		->setIcon(1, 5)
		->setDescriptionId("shovelStone")
		->handEquipped();

	Item::stoneHoe = NEW_ITEM(ITEM_HOE_STONE)
		->setIcon(1, 8)
		->setDescriptionId("hoeStone")
		->handEquipped();

	Item::ironSword = NEW_ITEM(ITEM_SWORD_IRON)
		->setIcon(2, 4)
		->setDescriptionId("swordIron")
		->handEquipped();

	Item::ironPickaxe = NEW_ITEM(ITEM_PICKAXE_IRON)
		->setIcon(2, 6)
		->setDescriptionId("pickaxeIron")
		->handEquipped();

	Item::ironAxe = NEW_ITEM(ITEM_HATCHET_IRON)
		->setIcon(2, 7)
		->setDescriptionId("hatchetIron")
		->handEquipped();

	Item::ironShovel = NEW_ITEM(ITEM_SHOVEL_IRON)
		->setIcon(2, 5)
		->setDescriptionId("shovelIron")
		->handEquipped();

	Item::ironHoe = NEW_ITEM(ITEM_HOE_IRON)
		->setIcon(2, 8)
		->setDescriptionId("hoeIron")
		->handEquipped();

	Item::goldSword = NEW_ITEM(ITEM_SWORD_GOLD)
		->setIcon(4, 4)
		->setDescriptionId("swordGold")
		->handEquipped();

	Item::goldPickaxe = NEW_ITEM(ITEM_PICKAXE_GOLD)
		->setIcon(4, 6)
		->setDescriptionId("pickaxeGold")
		->handEquipped();

	Item::goldAxe = NEW_ITEM(ITEM_HATCHET_GOLD)
		->setIcon(4, 7)
		->setDescriptionId("hatchetGold")
		->handEquipped();

	Item::goldShovel = NEW_ITEM(ITEM_SHOVEL_GOLD)
		->setIcon(4, 5)
		->setDescriptionId("shovelGold")
		->handEquipped();

	Item::goldHoe = NEW_ITEM(ITEM_HOE_GOLD)
		->setIcon(4, 8)
		->setDescriptionId("hoeGold")
		->handEquipped();

	Item::diamondSword = NEW_ITEM(ITEM_SWORD_DIAMOND)
		->setIcon(3, 4)
		->setDescriptionId("swordDiamond")
		->handEquipped();

	Item::diamondPickaxe = NEW_ITEM(ITEM_PICKAXE_DIAMOND)
		->setIcon(3, 6)
		->setDescriptionId("pickaxeDiamond")
		->handEquipped();

	Item::diamondAxe = NEW_ITEM(ITEM_HATCHET_DIAMOND)
		->setIcon(3, 7)
		->setDescriptionId("hatchetDiamond")
		->handEquipped();

	Item::diamondShovel = NEW_ITEM(ITEM_SHOVEL_DIAMOND)
		->setIcon(3, 5)
		->setDescriptionId("shovelDiamond")
		->handEquipped();

	Item::diamondHoe = NEW_ITEM(ITEM_HOE_DIAMOND)
		->setIcon(3, 8)
		->setDescriptionId("hoeDiamond")
		->handEquipped();

	Item::clothHelmet = NEW_ITEM(ITEM_HELMET_CLOTH)
		->setIcon(0, 0)
		->setDescriptionId("helmetCloth");

	Item::clothChestplate = NEW_ITEM(ITEM_CHESTPLATE_CLOTH)
		->setIcon(0, 1)
		->setDescriptionId("chestplateCloth");

	Item::clothLeggings = NEW_ITEM(ITEM_LEGGINGS_CLOTH)
		->setIcon(0, 2)
		->setDescriptionId("leggingsCloth");

	Item::clothBoots = NEW_ITEM(ITEM_BOOTS_CLOTH)
		->setIcon(0, 3)
		->setDescriptionId("bootsCloth");

	Item::chainHelmet = NEW_ITEM(ITEM_HELMET_CHAIN)
		->setIcon(1, 0)
		->setDescriptionId("helmetChain");

	Item::chainChestplate = NEW_ITEM(ITEM_CHESTPLATE_CHAIN)
		->setIcon(1, 1)
		->setDescriptionId("chestplateChain");

	Item::chainLeggings = NEW_ITEM(ITEM_LEGGINGS_CHAIN)
		->setIcon(1, 2)
		->setDescriptionId("leggingsChain");

	Item::chainBoots = NEW_ITEM(ITEM_BOOTS_CHAIN)
		->setIcon(1, 3)
		->setDescriptionId("bootsChain");

	Item::ironHelmet = NEW_ITEM(ITEM_HELMET_IRON)
		->setIcon(2, 0)
		->setDescriptionId("helmetIron");

	Item::ironChestplate = NEW_ITEM(ITEM_CHESTPLATE_IRON)
		->setIcon(2, 1)
		->setDescriptionId("chestplateIron");

	Item::ironLeggings = NEW_ITEM(ITEM_LEGGINGS_IRON)
		->setIcon(2, 2)
		->setDescriptionId("leggingsIron");

	Item::ironBoots = NEW_ITEM(ITEM_BOOTS_IRON)
		->setIcon(2, 3)
		->setDescriptionId("bootsIron");

	Item::goldHelmet = NEW_ITEM(ITEM_HELMET_GOLD)
		->setIcon(4, 0)
		->setDescriptionId("helmetGold");

	Item::goldChestplate = NEW_ITEM(ITEM_CHESTPLATE_GOLD)
		->setIcon(4, 1)
		->setDescriptionId("chestplateGold");

	Item::goldLeggings = NEW_ITEM(ITEM_LEGGINGS_GOLD)
		->setIcon(4, 2)
		->setDescriptionId("leggingsGold");

	Item::goldBoots = NEW_ITEM(ITEM_BOOTS_GOLD)
		->setIcon(4, 3)
		->setDescriptionId("bootsGold");

	Item::diamondHelmet = NEW_ITEM(ITEM_HELMET_DIAMOND)
		->setIcon(3, 0)
		->setDescriptionId("helmetDiamond");

	Item::diamondChestplate = NEW_ITEM(ITEM_CHESTPLATE_DIAMOND)
		->setIcon(3, 1)
		->setDescriptionId("chestplateDiamond");

	Item::diamondLeggings = NEW_ITEM(ITEM_LEGGINGS_DIAMOND)
		->setIcon(3, 2)
		->setDescriptionId("leggingsDiamond");

	Item::diamondBoots = NEW_ITEM(ITEM_BOOTS_DIAMOND)
		->setIcon(3, 3)
		->setDescriptionId("bootsDiamond");

	Item::flintAndSteel = NEW_ITEM(ITEM_FLINT_AND_STEEL)
		->setIcon(5, 0)
		->setDescriptionId("flintAndSteel");

	Item::arrow = NEW_ITEM(ITEM_ARROW)
		->setIcon(5, 2)
		->setDescriptionId("arrow");

	Item::coal = NEW_ITEM(ITEM_COAL)
		->setIcon(7, 0)
		->setDescriptionId("coal");

	Item::diamond = NEW_ITEM(ITEM_DIAMOND)
		->setIcon(7, 3)
		->setDescriptionId("diamond");

	Item::ironIngot = NEW_ITEM(ITEM_INGOT_IRON)
		->setIcon(7, 1)
		->setDescriptionId("ingotIron");

	Item::goldIngot = NEW_ITEM(ITEM_INGOT_GOLD)
		->setIcon(7, 2)
		->setDescriptionId("ingotGold");

	Item::stick = NEW_ITEM(ITEM_STICK)
		->setIcon(5, 3)
		->handEquipped()
		->setDescriptionId("stick");

	Item::bowl = NEW_ITEM(ITEM_BOWL)
		->setIcon(7, 4)
		->setDescriptionId("bowl");

	Item::mushroomStew = NEW_ITEM(ITEM_STEW_MUSHROOM)
		->setIcon(8, 4)
		->setDescriptionId("mushroomStew");

	Item::string = NEW_ITEM(ITEM_STRING)
		->setIcon(8, 0)
		->setDescriptionId("string");

	Item::feather = NEW_ITEM(ITEM_FEATHER)
		->setIcon(8, 1)
		->setDescriptionId("feather");

	Item::sulphur = NEW_ITEM(ITEM_SULPHUR)
		->setIcon(8, 2)
		->setDescriptionId("sulphur");

	Item::seeds = NEW_ITEM(ITEM_SEEDS)
		->setIcon(9, 0)
		->setDescriptionId("seeds");

	Item::reeds = NEW_X_ITEM(TilePlanterItem, ITEM_REEDS, TILE_REEDS)
		->setIcon(11, 1)
		->setDescriptionId("reeds");

	Item::wheat = NEW_ITEM(ITEM_WHEAT)
		->setIcon(9, 1)
		->setDescriptionId("wheat");

	Item::bread = NEW_ITEM(ITEM_BREAD)
		->setIcon(9, 2)
		->setDescriptionId("bread");

	Item::flint = NEW_ITEM(ITEM_FLINT)
		->setIcon(6, 0)
		->setDescriptionId("flint");

	Item::rawPorkchop = NEW_ITEM(ITEM_PORKCHOP_RAW)
		->setIcon(7, 5)
		->setDescriptionId("porkchopRaw");

	Item::cookedPorkchop = NEW_ITEM(ITEM_PORKCHOP_COOKED)
		->setIcon(8, 5)
		->setDescriptionId("porkchopCooked");

	Item::apple = NEW_ITEM(ITEM_APPLE)
		->setIcon(10, 0)
		->setDescriptionId("appleGold");

	Item::goldApple = NEW_ITEM(ITEM_APPLE_GOLD)
		->setIcon(11, 0)
		->setDescriptionId("appleGold");

	Item::woodDoor = NEW_X_ITEM(DoorItem, ITEM_DOOR_WOOD, Material::wood)
		->setIcon(11, 2)
		->setDescriptionId("doorWood");

	Item::emptyBucket = NEW_ITEM(ITEM_BUCKET)
		->setIcon(10, 4)
		->setDescriptionId("bucket");

	Item::waterBucket = NEW_ITEM(ITEM_BUCKET_WATER)
		->setIcon(11, 4)
		->setDescriptionId("bucketWater");
		//->setCraftingRemainingItem(emptyBucket);

	Item::lavaBucket = NEW_ITEM(ITEM_BUCKET_LAVA)
		->setIcon(12, 4)
		->setDescriptionId("bucketLava");
		//>setCraftingRemainingItem(emptyBucket);

	Item::minecart = NEW_ITEM(ITEM_MINECART)
		->setIcon(7, 8)
		->setDescriptionId("minecart");

	Item::chestMinecart = NEW_ITEM(ITEM_MINECART_CHEST)
		->setIcon(7, 9)
		->setDescriptionId("minecartChest");

	Item::furnaceMinecart = NEW_ITEM(ITEM_MINECART_FURNACE)
		->setIcon(7, 10)
		->setDescriptionId("minecartFurnace");

	Item::boat = NEW_ITEM(ITEM_BOAT)
		->setIcon(8, 8)
		->setDescriptionId("boat");

	Item::ironDoor = NEW_X_ITEM(DoorItem, ITEM_DOOR_IRON, Material::metal)
		->setIcon(12, 2)
		->setDescriptionId("doorIron");
	
	Item::redStone = NEW_ITEM(ITEM_REDSTONE)
		->setIcon(8, 3)
		->setDescriptionId("redstone");

	Item::snowBall = NEW_ITEM(ITEM_SNOWBALL)
		->setIcon(14, 0)
		->setDescriptionId("snowball");

	Item::saddle = NEW_ITEM(ITEM_SADDLE)
		->setIcon(8, 6)
		->setDescriptionId("saddle");

	Item::leather = NEW_ITEM(ITEM_LEATHER)
		->setIcon(7, 6)
		->setDescriptionId("leather");

	Item::milk = NEW_ITEM(ITEM_BUCKET_MILK)
		->setIcon(13, 4)
		->setDescriptionId("milk");
		//->setCraftingRemainingItem(emptyBucket);

	Item::brick = NEW_ITEM(ITEM_BRICK)
		->setIcon(6, 1)
		->setDescriptionId("brick");

	Item::clay = NEW_ITEM(ITEM_CLAY)
		->setIcon(9, 3)
		->setDescriptionId("clay");

	Item::paper = NEW_ITEM(ITEM_PAPER)
		->setIcon(10, 3)
		->setDescriptionId("paper");

	Item::book = NEW_ITEM(ITEM_BOOK)
		->setIcon(11, 3)
		->setDescriptionId("book");

	Item::slimeBall = NEW_ITEM(ITEM_SLIME_BALL)
		->setIcon(14, 1)
		->setDescriptionId("slimeball");

	Item::egg = NEW_ITEM(ITEM_EGG)
		->setIcon(12, 0)
		->setDescriptionId("egg");

	Item::compass = NEW_ITEM(ITEM_COMPASS)
		->setIcon(6, 3)
		->setDescriptionId("compass");

	Item::fishingRod = NEW_ITEM(ITEM_FISHING_ROD)
		->setIcon(5, 4)
		->setDescriptionId("fishingRod");

	Item::dyePowder = NEW_ITEM(ITEM_DYE_POWDER)
		->setIcon(14, 4)
		->setDescriptionId("dyePowder");

	Item::clock = NEW_ITEM(ITEM_CLOCK)
		->setIcon(6, 4)
		->setDescriptionId("clock");

	Item::yellowDust = NEW_ITEM(ITEM_YELLOW_DUST)
		->setIcon(9, 4)
		->setDescriptionId("yellowDust");

	Item::rawFish = NEW_ITEM(ITEM_FISH_RAW)
		->setIcon(9, 5)
		->setDescriptionId("fishRaw");

	Item::cookedFish = NEW_ITEM(ITEM_FISH_COOKED)
		->setIcon(10, 5)
		->setDescriptionId("fishCooked");

	Item::bone = NEW_ITEM(ITEM_BONE)
		->setIcon(12, 1)
		->setDescriptionId("bone")
		->handEquipped();

	Item::sugar = NEW_ITEM(ITEM_SUGAR)
		->setIcon(13, 0)
		->setDescriptionId("sugar")
		->handEquipped(); // weirdly also in JE

	Item::cake = NEW_ITEM(ITEM_CAKE)
		->setIcon(13, 1)
		->setMaxStackSize(1)
		->setDescriptionId("cake");

	Item::bed = NEW_ITEM(ITEM_BED)
		->setIcon(13, 2)
		->setDescriptionId("bed");

	Item::diode = NEW_ITEM(ITEM_DIODE)
		->setIcon(6, 5)
		->setDescriptionId("diode");

	Item::cookie = NEW_ITEM(ITEM_COOKIE)
		->setMaxStackSize(8)
		->setIcon(12, 5)
		->setDescriptionId("cookie");

	Item::sign = NEW_ITEM(ITEM_SIGN)
		->setIcon(10, 2)
		->setDescriptionId("sign");

	Item::painting = NEW_ITEM(ITEM_PAINTING)
		->setIcon(10, 1)
		->setDescriptionId("painting");

	Item::map = NEW_ITEM(ITEM_MAP)
		->setIcon(12, 3)
		->setDescriptionId("map");

	Item::shears = NEW_ITEM(ITEM_SHEARS)
		->setIcon(13, 5)
		->setDescriptionId("shears");
	
	Item::record_01 = NEW_ITEM(ITEM_RECORD_01)
		->setIcon(0, 15)
		->setDescriptionId("record");

	Item::record_02 = NEW_ITEM(ITEM_RECORD_02)
		->setIcon(1, 15)
		->setDescriptionId("record");

	Item::camera = NEW_X_ITEMN(CameraItem, ITEM_CAMERA)
		->setIcon(2, 15)
		->setDescriptionId("camera");

	Item::rocket = NEW_X_ITEMN(RocketItem, ITEM_ROCKET)
		->setIcon(14, 2)
		->setDescriptionId("rocket");

	Item::quiver = NEW_ITEM(ITEM_QUIVER)
		->setIcon(6, 2)
		->setDescriptionId("quiver");
}

int Item::getIcon(const ItemStack* item) const
{
	return m_icon;
}

bool Item::useOn(ItemStack* instance, Level* level, const TilePos& pos, Facing::Name face) const
{
	return false;
}

bool Item::useOn(ItemStack* instance, Player* player, Level* level, const TilePos& pos, Facing::Name face) const
{
	return false;
}

float Item::getDestroySpeed(ItemStack* instance, Tile* tile) const
{
	return 1.0f;
}

ItemStack* Item::use(ItemStack* instance, Level* level, Player* player) const
{
	return instance;
}

int Item::getMaxStackSize() const
{
	return m_maxStackSize;
}

TileData Item::getLevelDataForAuxValue(int x) const
{
	return 0;
}

bool Item::isStackedByData() const
{
	return m_bStackedByData;
}

int Item::getMaxDamage() const
{
	return m_maxDamage;
}

void Item::hurtEnemy(ItemStack* instance, Mob* mob) const
{

}

void Item::mineBlock(ItemStack* instance, const TilePos& pos, Facing::Name face) const
{

}

int Item::getAttackDamage(Entity* ent) const
{
	return 1;
}

bool Item::canDestroySpecial(Tile* tile) const
{
	return false;
}

void Item::interactEnemy(ItemStack* instance, Mob* mob) const
{

}

bool Item::isHandEquipped() const
{
	return m_bHandEquipped;
}

bool Item::isMirroredArt() const
{
	return false;
}

std::string Item::getDescription() const
{
	return m_DescriptionID;
}

std::string Item::getDescription(ItemStack* inst) const
{
	return m_DescriptionID;
}

std::string Item::getDescriptionId() const
{
	return m_DescriptionID;
}

std::string Item::getDescriptionId(ItemStack* inst) const
{
	return m_DescriptionID;
}

Item* Item::getCraftingRemainingItem() const
{
	return m_pCraftingRemainingItem;
}

bool Item::hasCraftingRemainingItem() const
{
	return m_pCraftingRemainingItem != 0;
}

std::string Item::getName() const
{
	return getDescriptionId() + ".name";
}

std::string Item::getHovertextName() const
{
	return getName();
}

void Item::onCraftedBy(ItemStack*, Player*, Level*)
{
}

void Item::inventoryTick(ItemStack*, Level*, Entity*, int, bool)
{
}

bool Item::isDamageable() const
{
	return m_maxDamage > 0 && !m_bStackedByData;
}

int Item::buildIdAux(int16_t auxValue, const CompoundTag* userData) const
{
	return auxValue | (unsigned int)(m_itemID << 16);
}

Item
	*Item::ironShovel,
	*Item::ironPickaxe,
	*Item::ironAxe,
	*Item::flintAndSteel,
	*Item::apple,
	*Item::bow,
	*Item::arrow,
	*Item::coal,
	*Item::diamond,
	*Item::ironIngot,
	*Item::goldIngot,
	*Item::ironSword,
	*Item::woodSword,
	*Item::woodShovel,
	*Item::woodPickaxe,
	*Item::woodAxe,
	*Item::stoneSword,
	*Item::stoneShovel,
	*Item::stonePickaxe,
	*Item::stoneAxe,
	*Item::diamondSword,
	*Item::diamondShovel,
	*Item::diamondPickaxe,
	*Item::diamondAxe,
	*Item::stick,
	*Item::bowl,
	*Item::mushroomStew,
	*Item::goldSword,
	*Item::goldShovel,
	*Item::goldPickaxe,
	*Item::goldAxe,
	*Item::string,
	*Item::feather,
	*Item::sulphur,
	*Item::woodHoe,
	*Item::stoneHoe,
	*Item::ironHoe,
	*Item::diamondHoe,
	*Item::goldHoe,
	*Item::seeds,
	*Item::wheat,
	*Item::bread,
	*Item::clothHelmet,
	*Item::clothChestplate,
	*Item::clothLeggings,
	*Item::clothBoots,
	*Item::chainHelmet,
	*Item::chainChestplate,
	*Item::chainLeggings,
	*Item::chainBoots,
	*Item::ironHelmet,
	*Item::ironChestplate,
	*Item::ironLeggings,
	*Item::ironBoots,
	*Item::diamondHelmet,
	*Item::diamondChestplate,
	*Item::diamondLeggings,
	*Item::diamondBoots,
	*Item::goldHelmet,
	*Item::goldChestplate,
	*Item::goldLeggings,
	*Item::goldBoots,
	*Item::flint,
	*Item::rawPorkchop,
	*Item::cookedPorkchop,
	*Item::painting,
	*Item::goldApple,
	*Item::sign,
	*Item::woodDoor,
	*Item::emptyBucket,
	*Item::waterBucket,
	*Item::lavaBucket,
	*Item::minecart,
	*Item::saddle,
	*Item::ironDoor,
	*Item::redStone,
	*Item::snowBall,
	*Item::boat,
	*Item::leather,
	*Item::milk,
	*Item::brick,
	*Item::clay,
	*Item::reeds,
	*Item::paper,
	*Item::book,
	*Item::slimeBall,
	*Item::chestMinecart,
	*Item::furnaceMinecart,
	*Item::egg,
	*Item::compass,
	*Item::fishingRod,
	*Item::clock,
	*Item::yellowDust,
	*Item::rawFish,
	*Item::cookedFish,
	*Item::dyePowder,
	*Item::bone,
	*Item::sugar,
	*Item::cake,
	*Item::bed,
	*Item::diode,
	*Item::cookie,
	*Item::map,
	*Item::shears,
	*Item::record_01,
	*Item::record_02,
	*Item::camera,
	*Item::rocket,
	*Item::quiver;

Item::Tier
	Item::Tier::WOOD   (0, 59,   2.0f,  0),
	Item::Tier::STONE  (1, 131,  4.0f,  1),
	Item::Tier::IRON   (2, 250,  6.0f,  2),
	Item::Tier::DIAMOND(3, 1561, 8.0f,  3),
	Item::Tier::GOLD   (0, 32,   12.0f, 0);

/*Item::ArmorTier
	Item::ArmorTier::CLOTH(0, "armor/cloth_1.png", "armor/cloth_2.png"),
	Item::ArmorTier::CHAIN(1, "armor/chain_1.png", "armor/chain_2.png"),
	Item::ArmorTier::IRON(2, "armor/iron_1.png", "armor/iron_2.png"),
	Item::ArmorTier::DIAMOND(3, "armor/diamond_1.png", "armor/diamond_2.png"),
	Item::ArmorTier::GOLD(1, "armor/gold_1.png", "armor/gold_2.png");*/

std::string Item::ICON_DESCRIPTION_PREFIX = "item.";
