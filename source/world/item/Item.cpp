/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "Item.hpp"

#include "CameraItem.hpp"
#include "DoorItem.hpp"
#include "TileItem.hpp"
#include "TilePlanterItem.hpp"
#include "RocketItem.hpp"

#define ITEM(x) ((x) - 256)

#define NEW_ITEM(id) (new Item(ITEM(id)))
#define NEW_X_ITEM(Type, id, ...) (new Type(ITEM(id), __VA_ARGS__))
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

void Item::initItems()
{
	if (g_bInittedItems)
		return;

	g_bInittedItems = true;

	// @TODO: Add missing items
	Item::bow = NEW_ITEM(/*BowItem, */ITEM_BOW)
		->setIcon(5, 1)
		->setDescriptionId("bow");

	Item::sword_wood = NEW_ITEM(/*WeaponItem, */ITEM_SWORD_WOOD, Tier::WOOD)
		->setIcon(0, 4)
		->setDescriptionId("swordWood")
		->handEquipped();

	Item::pickAxe_wood = NEW_ITEM(/*PickaxeItem, */ITEM_PICKAXE_WOOD, Tier::WOOD)
		->setIcon(0, 6)
		->setDescriptionId("pickaxeWood")
		->handEquipped();

	Item::hatchet_wood = NEW_ITEM(/*AxeItem, */ITEM_HATCHET_WOOD, Tier::WOOD)
		->setIcon(0, 7)
		->setDescriptionId("hatchetWood")
		->handEquipped();

	Item::shovel_wood = NEW_ITEM(/*ShovelItem, */ITEM_SHOVEL_WOOD, Tier::WOOD)
		->setIcon(0, 5)
		->setDescriptionId("shovelWood")
		->handEquipped();

	Item::hoe_wood = NEW_ITEM(/*HoeItem, */ITEM_HOE_WOOD, Tier::WOOD)
		->setIcon(0, 8)
		->setDescriptionId("hoeWood")
		->handEquipped();

	Item::sword_stone = NEW_ITEM(/*WeaponItem, */ITEM_SWORD_STONE, Tier::STONE)
		->setIcon(1, 4)
		->setDescriptionId("swordStone")
		->handEquipped();

	Item::pickAxe_stone = NEW_ITEM(/*PickaxeItem, */ITEM_PICKAXE_STONE, Tier::STONE)
		->setIcon(1, 6)
		->setDescriptionId("pickaxeStone")
		->handEquipped();

	Item::hatchet_stone = NEW_ITEM(/*AxeItem, */ITEM_HATCHET_STONE, Tier::STONE)
		->setIcon(1, 7)
		->setDescriptionId("hatchetStone")
		->handEquipped();

	Item::shovel_stone = NEW_ITEM(/*ShovelItem, */ITEM_SHOVEL_STONE, Tier::STONE)
		->setIcon(1, 5)
		->setDescriptionId("shovelStone")
		->handEquipped();

	Item::hoe_stone = NEW_ITEM(/*HoeItem, */ITEM_HOE_STONE, Tier::STONE)
		->setIcon(1, 8)
		->setDescriptionId("hoeStone")
		->handEquipped();

	Item::sword_iron = NEW_ITEM(/*WeaponItem, */ITEM_SWORD_IRON, Tier::IRON)
		->setIcon(2, 4)
		->setDescriptionId("swordIron")
		->handEquipped();

	Item::pickAxe_iron = NEW_ITEM(/*PickaxeItem, */ITEM_PICKAXE_IRON, Tier::IRON)
		->setIcon(2, 6)
		->setDescriptionId("pickaxeIron")
		->handEquipped();

	Item::hatchet_iron = NEW_ITEM(/*AxeItem, */ITEM_HATCHET_IRON, Tier::IRON)
		->setIcon(2, 7)
		->setDescriptionId("hatchetIron")
		->handEquipped();

	Item::shovel_iron= NEW_ITEM(/*ShovelItem, */ITEM_SHOVEL_IRON, Tier::IRON)
		->setIcon(2, 5)
		->setDescriptionId("shovelIron")
		->handEquipped();

	Item::hoe_iron= NEW_ITEM(/*HoeItem, */ITEM_HOE_IRON, Tier::IRON)
		->setIcon(2, 8)
		->setDescriptionId("hoeIron")
		->handEquipped();

	Item::sword_gold = NEW_ITEM(/*WeaponItem, */ITEM_SWORD_GOLD, Tier::GOLD)
		->setIcon(4, 4)
		->setDescriptionId("swordGold")
		->handEquipped();

	Item::pickAxe_gold = NEW_ITEM(/*PickaxeItem, */ITEM_PICKAXE_GOLD, Tier::GOLD)
		->setIcon(4, 6)
		->setDescriptionId("pickaxeGold")
		->handEquipped();

	Item::hatchet_gold = NEW_ITEM(/*AxeItem, */ITEM_HATCHET_GOLD, Tier::GOLD)
		->setIcon(4, 7)
		->setDescriptionId("hatchetGold")
		->handEquipped();

	Item::shovel_gold = NEW_ITEM(/*ShovelItem, */ITEM_SHOVEL_GOLD, Tier::GOLD)
		->setIcon(4, 5)
		->setDescriptionId("shovelGold")
		->handEquipped();

	Item::hoe_gold = NEW_ITEM(/*HoeItem, */ITEM_HOE_GOLD, Tier::GOLD)
		->setIcon(4, 8)
		->setDescriptionId("hoeGold")
		->handEquipped();

	Item::sword_emerald = NEW_ITEM(/*WeaponItem, */ITEM_SWORD_EMERALD, Tier::EMERALD)
		->setIcon(3, 4)
		->setDescriptionId("swordEmerald")
		->handEquipped();

	Item::pickAxe_emerald = NEW_ITEM(/*PickaxeItem, */ITEM_PICKAXE_EMERALD, Tier::EMERALD)
		->setIcon(3, 6)
		->setDescriptionId("pickaxeEmerald")
		->handEquipped();

	Item::hatchet_emerald = NEW_ITEM(/*AxeItem, */ITEM_HATCHET_EMERALD, Tier::EMERALD)
		->setIcon(3, 7)
		->setDescriptionId("hatchetEmerald")
		->handEquipped();

	Item::shovel_emerald = NEW_ITEM(/*ShovelItem, */ITEM_SHOVEL_EMERALD, Tier::EMERALD)
		->setIcon(3, 5)
		->setDescriptionId("shovelEmerald")
		->handEquipped();

	Item::hoe_emerald = NEW_ITEM(/*HoeItem, */ITEM_HOE_EMERALD, Tier::EMERALD)
		->setIcon(3, 8)
		->setDescriptionId("hoeEmerald")
		->handEquipped();

	Item::helmet_cloth = NEW_ITEM(/*ArmorItem, */ITEM_HELMET_CLOTH/*, ArmorTier::CLOTH, HEAD*/)
		->setIcon(0, 0)
		->setDescriptionId("helmetCloth");

	Item::chestplate_cloth = NEW_ITEM(/*ArmorItem, */ITEM_CHESTPLATE_CLOTH/*, ArmorTier::CLOTH, CHEST*/)
		->setIcon(0, 1)
		->setDescriptionId("chestplateCloth");

	Item::leggings_cloth = NEW_ITEM(/*ArmorItem, */ITEM_LEGGINGS_CLOTH/*, ArmorTier::CLOTH, LEGS*/)
		->setIcon(0, 2)
		->setDescriptionId("leggingsCloth");

	Item::boots_cloth = NEW_ITEM(/*ArmorItem, */ITEM_BOOTS_CLOTH/*, ArmorTier::CLOTH, FEET*/)
		->setIcon(0, 3)
		->setDescriptionId("bootsCloth");

	Item::helmet_iron = NEW_ITEM(/*ArmorItem, */ITEM_HELMET_IRON/*, ArmorTier::IRON, HEAD*/)
		->setIcon(2, 0)
		->setDescriptionId("helmetIron");

	Item::chestplate_iron = NEW_ITEM(/*ArmorItem, */ITEM_CHESTPLATE_IRON/*, ArmorTier::IRON, CHEST*/)
		->setIcon(2, 1)
		->setDescriptionId("chestplateIron");

	Item::leggings_iron = NEW_ITEM(/*ArmorItem, */ITEM_LEGGINGS_IRON/*, ArmorTier::IRON, LEGS*/)
		->setIcon(2, 2)
		->setDescriptionId("leggingsIron");

	Item::boots_iron = NEW_ITEM(/*ArmorItem, */ITEM_BOOTS_IRON/*, ArmorTier::IRON, FEET*/)
		->setIcon(2, 3)
		->setDescriptionId("bootsIron");

	Item::helmet_gold = NEW_ITEM(/*ArmorItem, */ITEM_HELMET_GOLD/*, ArmorTier::GOLD, HEAD*/)
		->setIcon(4, 0)
		->setDescriptionId("helmetGold");

	Item::chestplate_gold = NEW_ITEM(/*ArmorItem, */ITEM_CHESTPLATE_GOLD/*, ArmorTier::GOLD, CHEST*/)
		->setIcon(4, 1)
		->setDescriptionId("chestplateGold");

	Item::leggings_gold = NEW_ITEM(/*ArmorItem, */ITEM_LEGGINGS_GOLD/*, ArmorTier::GOLD, LEGS*/)
		->setIcon(4, 2)
		->setDescriptionId("leggingsGold");

	Item::boots_gold = NEW_ITEM(/*ArmorItem, */ITEM_BOOTS_EMERALD/*, ArmorTier::GOLD, FEET*/)
		->setIcon(4, 3)
		->setDescriptionId("bootsGold");

	Item::helmet_diamond = NEW_ITEM(/*ArmorItem, */ITEM_HELMET_EMERALD,/*, ArmorTier::EMERALD, HEAD*/)
		->setIcon(3, 0)
		->setDescriptionId("helmetEmerald");

	Item::chestplate_diamond = NEW_ITEM(/*ArmorItem, */ITEM_CHESTPLATE_EMERALD/*, ArmorTier::EMERALD, CHEST*/)
		->setIcon(3, 1)
		->setDescriptionId("chestplateEmerald");

	Item::leggings_diamond = NEW_ITEM(/*ArmorItem, */ITEM_LEGGINGS_EMERALD/*, ArmorTier::EMERALD, LEGS*/)
		->setIcon(3, 2)
		->setDescriptionId("leggingsEmerald");

	Item::boots_diamond = NEW_ITEM(/*ArmorItem, */ITEM_BOOTS_EMERALD/*, ArmorTier::EMERALD, FEET*/)
		->setIcon(3, 3)
		->setDescriptionId("bootsEmerald");

	Item::flintAndSteel = NEW_ITEM(/*FlintAndSteelItem, */ITEM_FLINT_AND_STEEL)
		->setIcon(5, 0)
		->setDescriptionId("flintAndSteel");

	Item::arrow = NEW_ITEM(ITEM_ARROW)
		->setIcon(5, 2)
		->setDescriptionId("arrow");

	Item::coal = NEW_ITEM(/*CoalItem, */ITEM_COAL)
		->setIcon(7, 0)
		->setDescriptionId("coal");

	Item::emerald = NEW_ITEM(ITEM_EMERALD)
		->setIcon(7, 3)
		->setDescriptionId("emerald");

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

	Item::mushroomStew = NEW_ITEM(/*BowlFoodItem, */ITEM_STEW_MUSHROOM/*, 10*/)
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

	Item::seeds = NEW_ITEM(/*SeedItem, */ITEM_SEEDS/*, TILE_WHEAT*/)
		->setIcon(9, 0)
		->setDescriptionId("seeds");

	Item::reeds = NEW_X_ITEM(TilePlanterItem, ITEM_REEDS, TILE_REEDS)
		->setIcon(11, 1)
		->setDescriptionId("reeds");

	Item::wheat = NEW_ITEM(ITEM_WHEAT)
		->setIcon(9, 1)
		->setDescriptionId("wheat");

	Item::bread = NEW_ITEM(/*FoodItem, */ITEM_BREAD/*, 5*/)
		->setIcon(9, 2)
		->setDescriptionId("bread");

	Item::flint = NEW_ITEM(ITEM_FLINT)
		->setIcon(6, 0)
		->setDescriptionId("flint");

	Item::porkChop_raw = NEW_ITEM(/*FoodItem, */ITEM_PORKCHOP_RAW/*, 3, true*/)
		->setIcon(7, 5)
		->setDescriptionId("porkchopRaw");

	Item::porkChop_cooked = NEW_ITEM(/*FoodItem, */ITEM_PORKCHOP_COOKED/*, 8, true*/)
		->setIcon(8, 5)
		->setDescriptionId("porkchopCooked");

	Item::apple = NEW_ITEM(/*FoodItem, */ITEM_APPLE/*, 4*/)
		->setIcon(10, 0)
		->setDescriptionId("appleGold");

	Item::apple_gold = NEW_ITEM(/*FoodItem, */ITEM_APPLE_GOLD/*, 42*/)
		->setIcon(11, 0)
		->setDescriptionId("appleGold");

	Item::door_wood = NEW_X_ITEM(DoorItem, ITEM_DOOR_WOOD, Material::wood)
		->setIcon(11, 2)
		->setDescriptionId("doorWood");

	Item::bucket_empty = NEW_ITEM(/*BucketItem, */ITEM_BUCKET/*, TILE_AIR*/)
		->setIcon(10, 4)
		->setDescriptionId("bucket");

	Item::bucket_water = NEW_ITEM(/*BucketItem, */ITEM_BUCKET_WATER/*, TILE_WATER*/)
		->setIcon(11, 4)
		->setDescriptionId("bucketWater");
		//->setCraftingRemainingItem(emptyBucket);

	Item::bucket_lava = NEW_ITEM(/*BucketItem, */ITEM_BUCKET_LAVA/*, TILE_LAVA*/)
		->setIcon(12, 4)
		->setDescriptionId("bucketLava");
		//>setCraftingRemainingItem(emptyBucket);

	Item::minecart = NEW_ITEM(/*MinecartItem, */ITEM_MINECART/*, 0*/)
		->setIcon(7, 8)
		->setDescriptionId("minecart");

	Item::minecart_chest = NEW_ITEM(/*MinecartItem, */ITEM_MINECART_CHEST/*, 1*/)
		->setIcon(7, 9)
		->setDescriptionId("minecartChest");

	Item::minecart_furnace = NEW_ITEM(/*MinecartItem, */ITEM_MINECART_FURNACE/*, 2*/)
		->setIcon(7, 10)
		->setDescriptionId("minecartFurnace");

	Item::boat = NEW_ITEM(/*BoatItem, */ITEM_BOAT)
		->setIcon(8, 8)
		->setDescriptionId("boat");

	Item::door_iron = NEW_X_ITEM(DoorItem, ITEM_DOOR_IRON, Material::metal)
		->setIcon(12, 2)
		->setDescriptionId("doorIron");
	
	Item::redStone = NEW_ITEM(/*TilePlanterItem, */ITEM_REDSTONE/*, TILE_WIRE, false*/)
		->setIcon(8, 3)
		->setDescriptionId("redstone");

	Item::snowBall = NEW_ITEM(/*SnowballItem, */ITEM_SNOWBALL)
		->setIcon(14, 0)
		->setDescriptionId("snowball");

	Item::saddle = NEW_ITEM(/*SaddleItem, */ITEM_SADDLE)
		->setIcon(8, 6)
		->setDescriptionId("saddle");

	Item::leather = NEW_ITEM(ITEM_LEATHER)
		->setIcon(7, 6)
		->setDescriptionId("leather");

	Item::milk = NEW_ITEM(/*BucketItem, */ITEM_BUCKET_MILK/*, -1*/)
		->setIcon(13, 4)
		->setDescriptionId("milk");
		//->setCraftingRemainingItem(emptyBucket);

	Item::brick = NEW_ITEM(ITEM_BRICK)
		->setIcon(6, 1)
		->setDescriptionId("brick");

	Item::clay = NEW_ITEM(ITEM_CLAY)
		->setIcon(9, 3)
		->setDescriptionId("clay");

	Item::reeds = NEW_X_ITEM(TilePlanterItem, ITEM_REEDS, TILE_REEDS)
		->setIcon(11, 1)
		->setDescriptionId("reeds");

	Item::paper = NEW_ITEM(ITEM_PAPER)
		->setIcon(10, 3)
		->setDescriptionId("paper");

	Item::book = NEW_ITEM(ITEM_BOOK)
		->setIcon(11, 3)
		->setDescriptionId("book");

	Item::slimeBall = NEW_ITEM(ITEM_SLIME_BALL)
		->setIcon(14, 1)
		->setDescriptionId("slimeball");

	Item::egg = NEW_ITEM(/*EggItem, */ITEM_EGG)
		->setIcon(12, 0)
		->setDescriptionId("egg");

	Item::compass = NEW_ITEM(ITEM_COMPASS)
		->setIcon(6, 3)
		->setDescriptionId("compass");

	Item::fishingRod = NEW_ITEM(/*FishingRodItem, */ITEM_FISHING_ROD)
		->setIcon(5, 4)
		->setDescriptionId("fishingRod");

	Item::dye_powder = NEW_ITEM(/*DyeItem, */ITEM_DYE_POWDER)
		->setIcon(14, 4)
		->setDescriptionId("dyePowder");

	Item::clock = NEW_ITEM(ITEM_CLOCK)
		->setIcon(6, 4)
		->setDescriptionId("clock");

	Item::yellowDust = NEW_ITEM(ITEM_YELLOW_DUST)
		->setIcon(9, 4)
		->setDescriptionId("yellowDust");

	Item::fish_raw = NEW_ITEM(/*FoodItem, */ITEM_FISH_RAW/*, 2*/)
		->setIcon(9, 5)
		->setDescriptionId("fishRaw");

	Item::fish_cooked = NEW_ITEM(/*FoodItem, */ITEM_FISH_COOKED/*, 5*/)
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

	Item::cake = NEW_ITEM(/*TilePlanterItem, */ITEM_CAKE/*, TILE_CAKE*/)
		->setIcon(13, 1)
		->setMaxStackSize(1)
		->setDescriptionId("cake");

	Item::bed = NEW_ITEM(/*BedItem, */ITEM_BED)
		->setIcon(13, 2)
		->setDescriptionId("bed");

	Item::diode = NEW_ITEM(/*TilePlanterItem, */ITEM_DIODE, /*TILE_REPEATER_OFF*/)
		->setIcon(6, 5)
		->setDescriptionId("diode");

	Item::sign = NEW_ITEM(/*SignItem, */ITEM_SIGN)
		->setIcon(10, 2)
		->setDescriptionId("sign");

	Item::painting = NEW_ITEM(/*PaintingItem, */ITEM_PAINTING)
		->setIcon(10, 1)
		->setDescriptionId("painting");

	Item::record_01 = NEW_ITEM(/*RecordItem, */ITEM_RECORD_01/*, "13"*/)
		->setIcon(0, 15)
		->setDescriptionId("record");

	Item::record_02 = NEW_ITEM(/*RecordItem, */ITEM_RECORD_02/*, "cat"*/)
		->setIcon(1, 15)
		->setDescriptionId("record");

	Item::camera = NEW_X_ITEMN(CameraItem, ITEM_CAMERA)
		->setIcon(2, 15)
		->setDescriptionId("camera");

	Item::rocket = NEW_X_ITEMN(RocketItem, ITEM_ROCKET)
		->setIcon(14, 2)
		->setDescriptionId("rocket");

	//Item::cookie = NEW_ITEM(/*FoodItem, */ITEM_COOKIE/*, 1*/)
	//	->setMaxStackSize(8)
	//	->setIcon(12, 5)
	//	->setDescriptionId("cookie");
	
	//Item::map = NEW_ITEM(/*MapItem, */ITEM_MAP)
	//	->setIcon(12, 3)
	//	->setDescriptionId("map");

	//Item::shears = NEW_ITEM(/*ShearsItem, */ITEM_SHEARS)
	//	->setIcon(13, 5)
	//	->setDescriptionId("shears");

	//Item::helmet_chain = NEW_ITEM(/*ArmorItem, */ITEM_HELMET_CHAIN/*, ArmorTier::CHAIN, HEAD*/)
	//	->setIcon(1, 0)
	//	->setDescriptionId("helmetChain");

	//Item::chestplate_chain = NEW_ITEM(/*ArmorItem, */ITEM_CHESTPLATE_CHAIN/*, ArmorTier::CHAIN, CHEST*/)
	//	->setIcon(1, 1)
	//	->setDescriptionId("chestplateChain");

	//Item::leggings_chain = NEW_ITEM(/*ArmorItem, */ITEM_LEGGINGS_CHAIN/*, ArmorTier::CHAIN, LEGS*/)
	//	->setIcon(1, 2)
	//	->setDescriptionId("leggingsChain");

	//Item::boots_chain = NEW_ITEM(/*ArmorItem, */ITEM_BOOTS_CHAIN/*, ArmorTier::CHAIN, FEET*/)
	//	->setIcon(1, 3)
	//	->setDescriptionId("bootsChain");

	//Item::quiver = NEW_ITEM(/*QuiverItem, */ITEM_QUIVER)
	//	->setIcon(6, 2)
	//	->setDescriptionId("quiver");
}

int Item::getIcon(const ItemInstance* pInstance) const
{
	return m_icon;
}

bool Item::useOn(ItemInstance* instance, Level* level, const TilePos& pos, Facing::Name face)
{
	return false;
}

bool Item::useOn(ItemInstance* instance, Player* player, Level* level, const TilePos& pos, Facing::Name face)
{
	return false;
}

float Item::getDestroySpeed(ItemInstance* instance, Tile* tile)
{
	return 1.0f;
}

ItemInstance* Item::use(ItemInstance* instance, Level* level, Player* player)
{
	return instance;
}

int Item::getMaxStackSize()
{
	return m_maxStackSize;
}

int Item::getLevelDataForAuxValue(int x)
{
	return 0;
}

bool Item::isStackedByData()
{
	return m_bStackedByData;
}

int Item::getMaxDamage()
{
	return m_maxDamage;
}

void Item::hurtEnemy(ItemInstance* instance, Mob* mob)
{

}

void Item::mineBlock(ItemInstance* instance, const TilePos& pos, Facing::Name face)
{

}

int Item::getAttackDamage(Entity* ent)
{
	return 1;
}

bool Item::canDestroySpecial(Tile* tile)
{
	return false;
}

void Item::interactEnemy(ItemInstance* instance, Mob* mob)
{

}

bool Item::isHandEquipped()
{
	return m_bHandEquipped;
}

bool Item::isMirroredArt()
{
	return false;
}

std::string Item::getDescription()
{
	return m_DescriptionID;
}

std::string Item::getDescription(ItemInstance* inst)
{
	return m_DescriptionID;
}

std::string Item::getDescriptionId()
{
	return m_DescriptionID;
}

std::string Item::getDescriptionId(ItemInstance* inst)
{
	return m_DescriptionID;
}

Item* Item::getCraftingRemainingItem()
{
	return m_pCraftingRemainingItem;
}

bool Item::hasCraftingRemainingItem()
{
	return m_pCraftingRemainingItem != 0;
}

std::string Item::getName()
{
	return getDescriptionId() + ".name";
}

int Item::buildIdAux(int16_t auxValue, const CompoundTag* userData)
{
	return auxValue | (unsigned int)(m_itemID << 16);
}

Item
	*Item::shovel_iron,
	*Item::pickAxe_iron,
	*Item::hatchet_iron,
	*Item::flintAndSteel,
	*Item::apple,
	*Item::bow,
	*Item::arrow,
	*Item::coal,
	*Item::emerald,
	*Item::ironIngot,
	*Item::goldIngot,
	*Item::sword_iron,
	*Item::sword_wood,
	*Item::shovel_wood,
	*Item::pickAxe_wood,
	*Item::hatchet_wood,
	*Item::sword_stone,
	*Item::shovel_stone,
	*Item::pickAxe_stone,
	*Item::hatchet_stone,
	*Item::sword_emerald,
	*Item::shovel_emerald,
	*Item::pickAxe_emerald,
	*Item::hatchet_emerald,
	*Item::stick,
	*Item::bowl,
	*Item::mushroomStew,
	*Item::sword_gold,
	*Item::shovel_gold,
	*Item::pickAxe_gold,
	*Item::hatchet_gold,
	*Item::string,
	*Item::feather,
	*Item::sulphur,
	*Item::hoe_wood,
	*Item::hoe_stone,
	*Item::hoe_iron,
	*Item::hoe_emerald,
	*Item::hoe_gold,
	*Item::seeds,
	*Item::wheat,
	*Item::bread,
	*Item::helmet_cloth,
	*Item::chestplate_cloth,
	*Item::leggings_cloth,
	*Item::boots_cloth,
	*Item::helmet_iron,
	*Item::chestplate_iron,
	*Item::leggings_iron,
	*Item::boots_iron,
	*Item::helmet_diamond,
	*Item::chestplate_diamond,
	*Item::leggings_diamond,
	*Item::boots_diamond,
	*Item::helmet_gold,
	*Item::chestplate_gold,
	*Item::leggings_gold,
	*Item::boots_gold,
	*Item::flint,
	*Item::porkChop_raw,
	*Item::porkChop_cooked,
	*Item::painting,
	*Item::apple_gold,
	*Item::sign,
	*Item::door_wood,
	*Item::bucket_empty,
	*Item::bucket_water,
	*Item::bucket_lava,
	*Item::minecart,
	*Item::saddle,
	*Item::door_iron,
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
	*Item::minecart_chest,
	*Item::minecart_furnace,
	*Item::egg,
	*Item::compass,
	*Item::fishingRod,
	*Item::clock,
	*Item::yellowDust,
	*Item::fish_raw,
	*Item::fish_cooked,
	*Item::dye_powder,
	*Item::bone,
	*Item::sugar,
	*Item::cake,
	*Item::bed,
	*Item::diode,
	*Item::record_01,
	*Item::record_02,
	*Item::camera,
	*Item::rocket;

Item::Tier
	Item::Tier::WOOD   (0, 59,   2.0f,  0),
	Item::Tier::STONE  (1, 131,  4.0f,  1),
	Item::Tier::IRON   (2, 250,  6.0f,  2),
	Item::Tier::EMERALD(3, 1561, 8.0f,  3),
	Item::Tier::GOLD   (0, 32,   12.0f, 0);

std::string Item::ICON_DESCRIPTION_PREFIX = "item.";
