/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "Item.hpp"

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
		printf("Item conflict id @ %d! Id already used\n", m_itemID);
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
		puts("Max stack size must be 1 for items with crafting results");

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

	Item::arrow = NEW_ITEM(ITEM_ARROW)
		->setIcon(5, 2)
		->setDescriptionId("arrow");

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

	Item::string = NEW_ITEM(ITEM_STRING)
		->setIcon(8, 0)
		->setDescriptionId("string");

	Item::feather = NEW_ITEM(ITEM_FEATHER)
		->setIcon(8, 1)
		->setDescriptionId("feather");

	Item::sulphur = NEW_ITEM(ITEM_SULPHUR)
		->setIcon(8, 2)
		->setDescriptionId("sulphur");

	Item::wheat = NEW_ITEM(ITEM_WHEAT)
		->setIcon(9, 1)
		->setDescriptionId("wheat");

	Item::door_wood = NEW_X_ITEM(DoorItem, ITEM_DOOR_WOOD, Material::wood)
		->setIcon(11, 2)
		->setDescriptionId("doorWood");

	Item::door_iron = NEW_X_ITEM(DoorItem, ITEM_DOOR_IRON, Material::metal)
		->setIcon(12, 2)
		->setDescriptionId("doorIron");

	Item::leather = NEW_ITEM(ITEM_LEATHER)
		->setIcon(7, 6)
		->setDescriptionId("leather");

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

	Item::compass = NEW_ITEM(ITEM_COMPASS)
		->setIcon(6, 3)
		->setDescriptionId("clock");

	Item::clock = NEW_ITEM(ITEM_CLOCK)
		->setIcon(6, 4)
		->setDescriptionId("clock");

	Item::bone = NEW_ITEM(ITEM_BONE)
		->setIcon(12, 1)
		->setDescriptionId("bone")
		->handEquipped();

	Item::sugar = NEW_ITEM(ITEM_SUGAR)
		->setIcon(13, 0)
		->setDescriptionId("sugar")
		->handEquipped(); // weirdly also in JE

	Item::camera = NEW_X_ITEMN(CameraItem, ITEM_CAMERA)
		->setIcon(2, 15)
		->setDescriptionId("camera");
}

int Item::getIcon(ItemInstance* pInstance)
{
	return m_icon;
}

bool Item::useOn(ItemInstance* instance, Level* level, int x, int y, int z, int d)
{
	return false;
}

bool Item::useOn(ItemInstance* instance, Player* player, Level* level, int x, int y, int z, int d)
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

void Item::mineBlock(ItemInstance* instance, int x, int y, int z, int d)
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
	*Item::camera;

Item::Tier
	Item::Tier::WOOD   (0, 59,   2.0f,  0),
	Item::Tier::STONE  (1, 131,  4.0f,  1),
	Item::Tier::IRON   (2, 250,  6.0f,  2),
	Item::Tier::EMERALD(3, 1561, 8.0f,  3),
	Item::Tier::GOLD   (0, 32,   12.0f, 0);

std::string Item::ICON_DESCRIPTION_PREFIX = "item.";
