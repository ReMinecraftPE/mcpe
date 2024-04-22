/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "world/level/Level.hpp"
#include "world/item/TileItem.hpp"
#include "world/entity/ItemEntity.hpp"

// Include tile definitions here
#include "SandStoneTile.hpp"
#include "SandTile.hpp"
#include "SandStoneTile.hpp"
#include "HalfTransparentTile.hpp"
#include "GlassTile.hpp"
#include "GravelTile.hpp"
#include "StoneTile.hpp"
#include "DirtTile.hpp"
#include "GrassTile.hpp"
#include "IceTile.hpp"
#include "ClayTile.hpp"
#include "FarmTile.hpp"
#include "StoneSlabTile.hpp"
#include "ClothTile.hpp"
#include "Bush.hpp"
#include "Sapling.hpp"
#include "TopSnowTile.hpp"
#include "TreeTile.hpp"
#include "TransparentTile.hpp"
#include "LeafTile.hpp"
#include "OreTile.hpp"
#include "RedStoneOreTile.hpp"
#include "ReedTile.hpp"
#include "InvisibleTile.hpp"
#include "LiquidTileDynamic.hpp"
#include "LiquidTileStatic.hpp"
#include "ObsidianTile.hpp"
#include "MetalTile.hpp"
#include "StairTile.hpp"
#include "TorchTile.hpp"
#include "FireTile.hpp"
#include "LadderTile.hpp"
#include "TntTile.hpp"
#include "DoorTile.hpp"
#include "SpongeTile.hpp"
#include "BookshelfTile.hpp"
#include "WireTile.hpp"
#include "RocketLauncherTile.hpp"

std::string Tile::TILE_DESCRIPTION_PREFIX = "tile.";

Tile* Tile::tiles        [C_MAX_TILES];
int   Tile::lightBlock   [C_MAX_TILES];
int   Tile::lightEmission[C_MAX_TILES];
bool  Tile::shouldTick   [C_MAX_TILES];
bool  Tile::solid        [C_MAX_TILES];
bool  Tile::translucent  [C_MAX_TILES];
bool  Tile::isEntityTile [C_MAX_TILES];


void Tile::_init()
{
	m_TextureFrame = 1;
	m_pSound = nullptr;
	field_28 = 1.0f;
	field_30 = 0.6f;
	m_hardness = 0.0f;
	m_blastResistance = 0.0f;
	m_descriptionID = "";
}

void Tile::_init(int ID, Material* pMaterial, int texture)
{
	_init();

	m_ID = ID;
	m_TextureFrame = texture;
	m_pMaterial = pMaterial;
	m_pSound = &SOUND_NORMAL;
	m_aabb = m_aabbReturned = AABB(0, 0, 0, 1, 1, 1);

	if (tiles[m_ID])
		// @BUG: Printing &tiles[m_ID], but probably supposed to print tiles[m_ID]
		LOG_W("Slot %d is already occupied by %p when adding %p", m_ID, &tiles[m_ID], this);
}

Tile::~Tile()
{
}

bool Tile::isCubeShaped()
{
	return true;
}

std::string Tile::getDescriptionId()
{
	return m_descriptionID;
}

std::string Tile::getName()
{
	return getDescriptionId();
}

Tile* Tile::setSoundType(const Tile::SoundType& st)
{
	m_pSound = &st;
	return this;
}

Tile* Tile::setDescriptionId(const std::string& x)
{
	m_descriptionID = TILE_DESCRIPTION_PREFIX + x;
	return this;
}

Tile* Tile::setDestroyTime(float time)
{
	m_hardness = time;

	if (m_blastResistance < 5 * time)
		m_blastResistance = 5 * time;

	return this;
}

Tile* Tile::setExplodeable(float power)
{
	m_blastResistance = power;
	return this;
}

Tile* Tile::setLightBlock(int x)
{
	lightBlock[m_ID] = x;
	return this;
}

Tile* Tile::setLightEmission(float x)
{
	lightEmission[m_ID] = int(15 * x);
	return this;
}

Tile* Tile::setTicking(bool bTick)
{
	shouldTick[m_ID] = bTick;
	return this;
}

Tile* Tile::setShape(float a, float b, float c, float d, float e, float f)
{
	m_aabb = AABB(a, b, c, d, e, f);

	return this;
}

Tile* Tile::init()
{
	setShape(m_aabb.min.x, m_aabb.min.y, m_aabb.min.z, m_aabb.max.x, m_aabb.max.y, m_aabb.max.z);

	tiles[m_ID] = this;

	solid[m_ID] = isSolidRender();
	lightBlock[m_ID] = isSolidRender() ? 255 : 0;
	translucent[m_ID] = m_pMaterial->blocksLight();
	isEntityTile[m_ID] = 0;

	return this;
}

int Tile::getRenderShape()
{
	return 0;
}

void Tile::updateDefaultShape()
{
	
}

int Tile::getTexture(int x)
{
	return m_TextureFrame;
}

int Tile::getTexture(int x, int y)
{
	return getTexture(x);
}

int Tile::getTickDelay()
{
	return 10;
}

bool Tile::isSignalSource()
{
	return false;
}

int Tile::getRenderLayer()
{
	return LAYER_OPAQUE;
}

bool Tile::isSolidRender()
{
	return true;
}

bool Tile::mayPick()
{
	return true;
}

bool Tile::mayPick(int x, bool y)
{
	return mayPick();
}

int Tile::getResource(int x, Random* pRandom)
{
	return m_ID;
}

int Tile::getResourceCount(Random* pRandom)
{
	return 1;
}

int Tile::getSpawnResourcesAuxValue(int x)
{
	return 1;
}

void Tile::initTiles()
{
	Tile::sand = (new SandTile(TILE_SAND, TEXTURE_SAND, Material::sand))
		->init()
		->setDestroyTime(0.5f)
		->setSoundType(Tile::SOUND_SAND)
		->setDescriptionId("sand");

	Tile::sandStone = (new SandStoneTile(TILE_SANDSTONE, TEXTURE_SANDSTONE_SIDE, Material::stone))
		->init()
		->setSoundType(Tile::SOUND_STONE)
		->setDestroyTime(0.8f)
		->setDescriptionId("sandStone");

	Tile::stoneBrick = (new Tile(TILE_STONEBRICK, TEXTURE_STONEBRICK, Material::stone))
		->init()
		->setDestroyTime(2.0f)
		->setExplodeable(10.0f)
		->setSoundType(Tile::SOUND_STONE)
		->setDescriptionId("stonebrick");

	Tile::redBrick = (new Tile(TILE_BRICKS, TEXTURE_BRICKS, Material::stone))
		->init()
		->setDestroyTime(2.0f)
		->setExplodeable(10.0f)
		->setSoundType(Tile::SOUND_STONE)
		->setDescriptionId("brick");

	Tile::wood = (new Tile(TILE_WOOD, TEXTURE_PLANKS, Material::stone))
		->init()
		->setDestroyTime(2.0f)
		->setExplodeable(5.0f)
		->setSoundType(Tile::SOUND_WOOD)
		->setDescriptionId("wood");

	Tile::glass = (new GlassTile(TILE_GLASS, TEXTURE_GLASS, Material::glass))
		->init()
		->setDestroyTime(0.3f)
		->setSoundType(Tile::SOUND_GLASS)
		->setDescriptionId("glass");
	
	Tile::calmWater = (new LiquidTileStatic(TILE_WATER_CALM, Material::water))
		->init()
		->setDestroyTime(100.0f)
		->setLightBlock(3)
		->setDescriptionId("water");

	Tile::calmLava = (new LiquidTileStatic(TILE_LAVA_CALM, Material::lava))
		->init()
		->setDestroyTime(100.0f)
		->setLightEmission(1.0f)
		->setLightBlock(255)
		->setDescriptionId("lava");

	Tile::gravel = (new GravelTile(TILE_GRAVEL, TEXTURE_GRAVEL, Material::sand))
		->init()
		->setDestroyTime(0.6f)
		->setSoundType(Tile::SOUND_GRAVEL)
		->setDescriptionId("gravel");

	Tile::rock = (new StoneTile(TILE_STONE, TEXTURE_STONE, Material::stone))
		->init()
		->setDestroyTime(1.5f)
		->setExplodeable(10.0f)
		->setSoundType(Tile::SOUND_STONE)
		->setDescriptionId("stone");

	Tile::unbreakable = (new Tile(TILE_BEDROCK, TEXTURE_BEDROCK, Material::stone))
		->init()
		->setDestroyTime(-1.0f)
		->setExplodeable(6000000.f)
		->setSoundType(Tile::SOUND_STONE)
		->setDescriptionId("bedrock");
	
	Tile::dirt = (new DirtTile(TILE_DIRT, TEXTURE_DIRT, Material::dirt))
		->init()
		->setDestroyTime(0.5f)
		->setSoundType(Tile::SOUND_GRAVEL)
		->setDescriptionId("dirt");
	
	Tile::grass = (new GrassTile(TILE_GRASS, Material::dirt))
		->init()
		->setDestroyTime(0.6f)
		->setSoundType(Tile::SOUND_GRASS)
		->setDescriptionId("grass");
	
	Tile::ice = (new IceTile(TILE_ICE, TEXTURE_ICE, Material::ice))
		->init()
		->setDestroyTime(0.5f)
		->setLightBlock(3)
		->setSoundType(Tile::SOUND_GLASS)
		->setDescriptionId("ice");

	Tile::clay = (new ClayTile(TILE_CLAY, TEXTURE_CLAY, Material::clay))
		->init()
		->setDestroyTime(0.6f)
		->setSoundType(Tile::SOUND_GRAVEL)
		->setDescriptionId("clay");

	Tile::farmland = (new FarmTile(TILE_FARMLAND, Material::dirt))
		->init()
		->setDestroyTime(0.6f)
		->setSoundType(Tile::SOUND_GRAVEL)
		->setDescriptionId("farmland");

	Tile::stoneSlab = (new StoneSlabTile(TILE_STONESLAB_FULL, true))
		->init()
		->setDestroyTime(2.0f)
		->setExplodeable(10.0f)
		->setSoundType(Tile::SOUND_STONE)
		->setDescriptionId("stoneSlab");

	Tile::stoneSlabHalf = (new StoneSlabTile(TILE_STONESLAB_HALF, false))
		->init()
		->setDestroyTime(2.0f)
		->setExplodeable(10.0f)
		->setSoundType(Tile::SOUND_STONE)
		->setDescriptionId("stoneSlab");

	Tile::cloth = (new ClothTile(TILE_CLOTH, 0xCF))
		->init()
		->setDestroyTime(0.8f)
		->setSoundType(Tile::SOUND_CLOTH)
		->setDescriptionId("cloth");

	Tile::flower = (new Bush(TILE_FLOWER, TEXTURE_FLOWER))
		->init()
		->setDestroyTime(0.0f)
		->setSoundType(Tile::SOUND_GRASS)
		->setDescriptionId("flower");

	Tile::rose = (new Bush(TILE_ROSE, TEXTURE_ROSE))
		->init()
		->setDestroyTime(0.0f)
		->setSoundType(Tile::SOUND_GRASS)
		->setDescriptionId("rose");

	Tile::mushroom1 = (new Bush(TILE_MUSHROOM_1, TEXTURE_MUSHROOM_BROWN))
		->init()
		->setDestroyTime(0.0f)
		->setSoundType(Tile::SOUND_GRASS)
		->setLightEmission(0.125f)
		->setDescriptionId("mushroom");

	Tile::mushroom2 = (new Bush(TILE_MUSHROOM_2, TEXTURE_MUSHROOM_RED))
		->init()
		->setDestroyTime(0.0f)
		->setSoundType(Tile::SOUND_GRASS)
		->setDescriptionId("mushroom");

	Tile::topSnow = (new TopSnowTile(TILE_TOPSNOW, TEXTURE_SNOW, Material::snow))
		->init()
		->setDestroyTime(0.1f)
		->setSoundType(Tile::SOUND_CLOTH)
		->setDescriptionId("snow");

	Tile::treeTrunk = (new TreeTile(TILE_TREE_TRUNK))
		->init()
		->setDestroyTime(2.0f)
		->setSoundType(Tile::SOUND_WOOD)
		->setDescriptionId("log");

	Tile::leaves = (new LeafTile(TILE_LEAVES))
		->init()
		->setDestroyTime(0.2f)
		->setLightBlock(true)
		->setSoundType(Tile::SOUND_GRASS)
		->setDescriptionId("leaves");

	Tile::emeraldOre = (new OreTile(TILE_ORE_EMERALD, TEXTURE_ORE_EMERALD))
		->init()
		->setDestroyTime(3.0f)
		->setExplodeable(5.0f)
		->setSoundType(Tile::SOUND_STONE)
		->setDescriptionId("oreDiamond");

	Tile::redStoneOre = (new RedStoneOreTile(TILE_ORE_REDSTONE, TEXTURE_ORE_RED_STONE, false))
		->init()
		->setDestroyTime(3.0f)
		->setExplodeable(5.0f)
		->setSoundType(Tile::SOUND_STONE)
		->setDescriptionId("oreRedstone");

	Tile::redStoneOre_lit = (new RedStoneOreTile(TILE_ORE_REDSTONE_LIT, TEXTURE_ORE_RED_STONE, true))
		->init()
		->setDestroyTime(3.0f)
		->setLightEmission(0.625f)
		->setExplodeable(5.0f)
		->setSoundType(Tile::SOUND_STONE)
		->setDescriptionId("oreRedstone");

	Tile::goldOre = (new OreTile(TILE_ORE_GOLD, TEXTURE_ORE_GOLD))
		->init()
		->setDestroyTime(3.0f)
		->setExplodeable(5.0f)
		->setSoundType(Tile::SOUND_STONE)
		->setDescriptionId("oreGold");

	Tile::ironOre = (new OreTile(TILE_ORE_IRON, TEXTURE_ORE_IRON))
		->init()
		->setDestroyTime(3.0f)
		->setExplodeable(5.0f)
		->setSoundType(Tile::SOUND_STONE)
		->setDescriptionId("oreIron");

	Tile::coalOre = (new OreTile(TILE_ORE_COAL, TEXTURE_ORE_COAL))
		->init()
		->setDestroyTime(3.0f)
		->setExplodeable(5.0f)
		->setSoundType(Tile::SOUND_STONE)
		->setDescriptionId("oreCoal");

	Tile::lapisOre = (new OreTile(TILE_ORE_LAPIS, TEXTURE_ORE_LAPIS))
		->init()
		->setDestroyTime(3.0f)
		->setExplodeable(5.0f)
		->setSoundType(Tile::SOUND_STONE)
		->setDescriptionId("oreLapis");

	Tile::reeds = (new ReedTile(TILE_REEDS))
		->init()
		->setDestroyTime(0.0f)
		->setSoundType(Tile::SOUND_GRASS)
		->setDescriptionId("reeds");

	Tile::ladder = (new LadderTile(TILE_LADDER, TEXTURE_LADDER))
		->init()
		->setDestroyTime(0.4f)
		->setSoundType(Tile::SOUND_WOOD)
		->setDescriptionId("ladder");

	Tile::obsidian = (new ObsidianTile(TILE_OBSIDIAN, TEXTURE_OBSIDIAN, Material::stone))
		->init()
		->setDestroyTime(10.0f)
		->setExplodeable(2000.0f)
		->setSoundType(Tile::SOUND_STONE)
		->setDescriptionId("obsidian");

	Tile::tnt = (new TntTile(TILE_TNT, TEXTURE_TNT_SIDE))
		->init()
		->setDestroyTime(0.0f)
		->setSoundType(Tile::SOUND_GRASS)
		->setDescriptionId("tnt");
	
	Tile::torch = (new TorchTile(TILE_TORCH, TEXTURE_TORCH_LIT, Material::decoration))
		->init()
		->setDestroyTime(0.0f)
		->setLightEmission(15.0f / 16.0f)
		->setSoundType(Tile::SOUND_WOOD)
		->setDescriptionId("torch");

	Tile::water = (new LiquidTileDynamic(TILE_WATER, Material::water))
		->init()
		->setDestroyTime(100.0f)
		->setLightBlock(3)
		->setDescriptionId("water");

	Tile::lava = (new LiquidTileDynamic(TILE_LAVA, Material::lava))
		->init()
		->setDestroyTime(0.0f)
		->setLightEmission(1.0f)
		->setLightBlock(255)
		->setDescriptionId("lava");

	Tile::fire = (new FireTile(TILE_FIRE, TEXTURE_FIRE1))
		->init()
		->setDestroyTime(0.0f)
		->setLightEmission(1.0f)
		->setSoundType(Tile::SOUND_WOOD)  // @NOTE: Setting fire's sound to Wood
		->setDescriptionId("fire");

	Tile::invisible_bedrock = (new InvisibleTile(TILE_INVISIBLE, TEXTURE_STONE, Material::stone))
		->init()
		->setDestroyTime(-1.0f)
		->setExplodeable(6000000.0f);

	Tile::goldBlock = (new MetalTile(TILE_BLOCK_GOLD, TEXTURE_GOLD, Material::metal))
		->init()
		->setDestroyTime(3.0f)
		->setExplodeable(10.0f)
		->setSoundType(Tile::SOUND_METAL)
		->setDescriptionId("blockGold");

	Tile::ironBlock = (new MetalTile(TILE_BLOCK_IRON, TEXTURE_IRON, Material::metal))
		->init()
		->setDestroyTime(5.0f)
		->setExplodeable(10.0f)
		->setSoundType(Tile::SOUND_METAL)
		->setDescriptionId("blockIron");

	Tile::emeraldBlock = (new MetalTile(TILE_BLOCK_EMERALD, TEXTURE_EMERALD, Material::metal))
		->init()
		->setDestroyTime(5.0f)
		->setExplodeable(10.0f)
		->setSoundType(Tile::SOUND_METAL)
		->setDescriptionId("blockEmerald");

	Tile::stairs_wood = (new StairTile(TILE_STAIRS_WOOD, Tile::wood))
		->init()
		->setDescriptionId("stairsWood");

	Tile::stairs_stone = (new StairTile(TILE_STAIRS_STONE, Tile::stoneBrick))
		->init()
		->setDescriptionId("stairsStone");

	Tile::door_wood = (new DoorTile(TILE_DOOR_WOOD, Material::wood))
		->init()
		->setDestroyTime(3.0f)
		->setSoundType(Tile::SOUND_WOOD)
		->setDescriptionId("doorWood");

	Tile::door_iron = (new DoorTile(TILE_DOOR_IRON, Material::metal))
		->init()
		->setDestroyTime(5.0f)
		->setSoundType(Tile::SOUND_METAL)
		->setDescriptionId("doorIron");

	Tile::cloth_00 = (new ClothTile(TILE_CLOTH_00, 15))
		->init()
		->setDestroyTime(0.8f)
		->setSoundType(Tile::SOUND_CLOTH)
		->setDescriptionId("cloth");

	Tile::cloth_10 = (new ClothTile(TILE_CLOTH_10, 14))
		->init()
		->setDestroyTime(0.8f)
		->setSoundType(Tile::SOUND_CLOTH)
		->setDescriptionId("cloth");

	Tile::cloth_20 = (new ClothTile(TILE_CLOTH_20, 13))
		->init()
		->setDestroyTime(0.8f)
		->setSoundType(Tile::SOUND_CLOTH)
		->setDescriptionId("cloth");

	Tile::cloth_30 = (new ClothTile(TILE_CLOTH_30, 12))
		->init()
		->setDestroyTime(0.8f)
		->setSoundType(Tile::SOUND_CLOTH)
		->setDescriptionId("cloth");

	Tile::cloth_40 = (new ClothTile(TILE_CLOTH_40, 11))
		->init()
		->setDestroyTime(0.8f)
		->setSoundType(Tile::SOUND_CLOTH)
		->setDescriptionId("cloth");

	Tile::cloth_50 = (new ClothTile(TILE_CLOTH_50, 10))
		->init()
		->setDestroyTime(0.8f)
		->setSoundType(Tile::SOUND_CLOTH)
		->setDescriptionId("cloth");

	Tile::cloth_60 = (new ClothTile(TILE_CLOTH_60, 9))
		->init()
		->setDestroyTime(0.8f)
		->setSoundType(Tile::SOUND_CLOTH)
		->setDescriptionId("cloth");

	Tile::cloth_70 = (new ClothTile(TILE_CLOTH_70, 8))
		->init()
		->setDestroyTime(0.8f)
		->setSoundType(Tile::SOUND_CLOTH)
		->setDescriptionId("cloth");

	Tile::cloth_01 = (new ClothTile(TILE_CLOTH_01, 7))
		->init()
		->setDestroyTime(0.8f)
		->setSoundType(Tile::SOUND_CLOTH)
		->setDescriptionId("cloth");

	Tile::cloth_11 = (new ClothTile(TILE_CLOTH_11, 6))
		->init()
		->setDestroyTime(0.8f)
		->setSoundType(Tile::SOUND_CLOTH)
		->setDescriptionId("cloth");

	Tile::cloth_21 = (new ClothTile(TILE_CLOTH_21, 5))
		->init()
		->setDestroyTime(0.8f)
		->setSoundType(Tile::SOUND_CLOTH)
		->setDescriptionId("cloth");

	Tile::cloth_31 = (new ClothTile(TILE_CLOTH_31, 4))
		->init()
		->setDestroyTime(0.8f)
		->setSoundType(Tile::SOUND_CLOTH)
		->setDescriptionId("cloth");

	Tile::cloth_41 = (new ClothTile(TILE_CLOTH_41, 3))
		->init()
		->setDestroyTime(0.8f)
		->setSoundType(Tile::SOUND_CLOTH)
		->setDescriptionId("cloth");

	Tile::cloth_51 = (new ClothTile(TILE_CLOTH_51, 2))
		->init()
		->setDestroyTime(0.8f)
		->setSoundType(Tile::SOUND_CLOTH)
		->setDescriptionId("cloth");

	Tile::cloth_61 = (new ClothTile(TILE_CLOTH_61, 1))
		->init()
		->setDestroyTime(0.8f)
		->setSoundType(Tile::SOUND_CLOTH)
		->setDescriptionId("cloth");

	Tile::info_updateGame1 = (new DirtTile(TILE_INFO_UPDATEGAME1, TEXTURE_INFO_UPDATEGAME1, Material::dirt))
		->init()
		->setDestroyTime(5.0f)
		->setSoundType(Tile::SOUND_GRAVEL)
		->setDescriptionId("info_update");

	Tile::info_updateGame2 = (new DirtTile(TILE_INFO_UPDATEGAME2, TEXTURE_INFO_UPDATEGAME2, Material::dirt))
		->init()
		->setDestroyTime(5.0f)
		->setSoundType(Tile::SOUND_GRAVEL)
		->setDescriptionId("info_update");

	// custom additions here

	Tile::sapling = (new Sapling(TILE_SAPLING, TEXTURE_SAPLING))
		->init()
		->setDestroyTime(0.0f)
		->setSoundType(Tile::SOUND_GRASS)
		->setDescriptionId("sapling");

	Tile::sponge = (new SpongeTile(TILE_SPONGE, TEXTURE_SPONGE))
		->init()
		->setDestroyTime(0.5f)
		->setSoundType(Tile::SOUND_CLOTH)
		->setDescriptionId("sponge");

	Tile::lapisBlock = (new MetalTile(TILE_BLOCK_LAPIS, TEXTURE_LAPIS, Material::metal))
		->init()
		->setDestroyTime(3.0f)
		->setExplodeable(10.0f)
		->setSoundType(Tile::SOUND_METAL)
		->setDescriptionId("blockLapis");

	Tile::bookshelf = (new BookshelfTile(TILE_BOOKSHELF, TEXTURE_BOOKSHELF, Material::wood))
		->init()
		->setDestroyTime(1.5f)
		->setSoundType(Tile::SOUND_WOOD)
		->setDescriptionId("bookshelf");

	Tile::mossStone = (new Tile(TILE_MOSS_STONE, TEXTURE_MOSSY_STONE, Material::stone))
		->init()
		->setDestroyTime(2.0f)
		->setExplodeable(10.0f)
		->setSoundType(Tile::SOUND_STONE)
		->setDescriptionId("stoneMoss");

	Tile::cryingObsidian = (new Tile(TILE_OBSIDIAN_CRYING, TEXTURE_OBSIDIAN_CRYING, Material::stone))
		->init()
		->setDestroyTime(10.0f)
		->setExplodeable(2000.0f)
		->setSoundType(Tile::SOUND_STONE)
		->setDescriptionId("cryingObsidian");

	// Jolly
	Tile::rocketLauncher = (new RocketLauncherTile(TILE_ROCKET_LAUNCHER))
		->init()
		->setSoundType(Tile::SOUND_STONE)
		->setDescriptionId("rocketLauncher");

	for (int i = 0; i < C_MAX_TILES; i++)
	{
		if (Tile::tiles[i])
			Item::items[i] = new TileItem(i - C_MAX_TILES);
	}
}

void Tile::teardownTiles()
{
	for (int i = 0; i < C_MAX_TILES; i++)
		delete tiles[i];
}

void Tile::updateShape(LevelSource* a, int x, int y, int z)
{
}

void Tile::addLights(Level* p, int x, int y, int z)
{
}

float Tile::getBrightness(LevelSource* pSrc, int x, int y, int z)
{
	return pSrc->getBrightness(x, y, z);
}

int Tile::getColor(LevelSource* pSrc, int x, int y, int z)
{
	return 0xFFFFFF; // White
}

AABB* Tile::getAABB(Level* pLevel, int x, int y, int z)
{
	Vec3 offset((float)x, (float)y, (float)z);

	m_aabbReturned = AABB(offset + m_aabb.min, offset + m_aabb.max);
	return &m_aabbReturned;
}

AABB Tile::getTileAABB(Level* pLevel, int x, int y, int z)
{
	Vec3 offset((float)x, (float)y, (float)z);
	return AABB(offset + m_aabb.min, offset + m_aabb.max);
}

void Tile::addAABBs(Level* pLevel, int x, int y, int z, const AABB* aabb, std::vector<AABB>& out)
{
	AABB* pTileAABB = getAABB(pLevel, x, y, z);

	if (pTileAABB && pTileAABB->intersect(*aabb))
	{
		out.push_back(*pTileAABB);
	}
}

bool Tile::shouldRenderFace(LevelSource* pSrc, int x, int y, int z, int dir)
{
	//if ((y | x | z) > C_MAX_CHUNKS_Z * 16)
	//	return false;

	switch (dir)
	{
	case DIR_ZNEG:
		if (z == -1) return false;
		if (m_aabb.min.z > 0.0f) return true;
		break;
	case DIR_ZPOS:
		if (z == C_MAX_CHUNKS_Z * 16) return false;
		if (m_aabb.max.z < 1.0f) return true;
		break;
	case DIR_XNEG:
		if (x == -1) return false;
		if (m_aabb.min.x > 0.0f) return true;
		break;
	case DIR_XPOS:
		if (x == C_MAX_CHUNKS_X * 16) return false;
		if (m_aabb.max.x < 1.0f) return true;
		break;
	case DIR_YNEG:
		if (y == -1) return false;
		if (m_aabb.min.y > 0.0f) return true;
		break;
	case DIR_YPOS:
		if (m_aabb.max.y < 1.0f) return true;
		break;
	}

	Tile* pTile = Tile::tiles[pSrc->getTile(x, y, z)];
	if (!pTile)
		return true;

	if (dir == DIR_YPOS && pTile->m_ID == Tile::topSnow->m_ID)
		return false;

	return !pTile->isSolidRender();
}

int Tile::getTexture(LevelSource* pSrc, int x, int y, int z, int dir)
{
	return getTexture(dir, pSrc->getData(x, y, z));
}

bool Tile::canSurvive(Level* pLevel, int x, int y, int z)
{
	return true;
}

// returns if we can place over the tile
bool Tile::mayPlace(Level* pLevel, int x, int y, int z)
{
	TileID tile = pLevel->getTile(x, y, z);
	if (!tile)
		return true; // we can definitely place something over air
	
	return Tile::tiles[tile]->m_pMaterial->isLiquid();
}

void Tile::tick(Level* pLevel, int x, int y, int z, Random* pRandom)
{

}

void Tile::animateTick(Level* pLevel, int x, int y, int z, Random* pRandom)
{

}

void Tile::destroy(Level* pLevel, int x, int y, int z, int dir)
{

}

void Tile::neighborChanged(Level* pLevel, int x, int y, int z, int dir)
{

}

void Tile::onPlace(Level* pLevel, int x, int y, int z)
{

}

void Tile::onRemove(Level* pLevel, int x, int y, int z)
{

}

bool Tile::containsX(const Vec3& v)
{
	return v.y >= m_aabb.min.y
		&& v.y <= m_aabb.max.y
		&& v.z >= m_aabb.min.z
		&& v.z <= m_aabb.max.z;
}

bool Tile::containsY(const Vec3& v)
{
	return v.x >= m_aabb.min.x
		&& v.x <= m_aabb.max.x
		&& v.z >= m_aabb.min.z
		&& v.z <= m_aabb.max.z;
}

bool Tile::containsZ(const Vec3& v)
{
	return v.x >= m_aabb.min.x
		&& v.x <= m_aabb.max.x
		&& v.y >= m_aabb.min.y
		&& v.y <= m_aabb.max.y;
}

HitResult Tile::clip(Level* level, int x, int y, int z, Vec3 vec1, Vec3 vec2)
{
	updateShape(level, x, y, z);

	Vec3 clipMinX, clipMinY, clipMinZ;
	Vec3 clipMaxX, clipMaxY, clipMaxZ;
	bool bClipMinX, bClipMinY, bClipMinZ;
	bool bClipMaxX, bClipMaxY, bClipMaxZ;

	vec1 += Vec3(-float(x), -float(y), -float(z));
	vec2 += Vec3(-float(x), -float(y), -float(z));

	bClipMinX = vec1.clipX(vec2, m_aabb.min.x, clipMinX) && containsX(clipMinX);
	bClipMaxX = vec1.clipX(vec2, m_aabb.max.x, clipMaxX) && containsX(clipMaxX);
	bClipMinY = vec1.clipY(vec2, m_aabb.min.y, clipMinY) && containsY(clipMinY);
	bClipMaxY = vec1.clipY(vec2, m_aabb.max.y, clipMaxY) && containsY(clipMaxY);
	bClipMinZ = vec1.clipZ(vec2, m_aabb.min.z, clipMinZ) && containsZ(clipMinZ);
	bClipMaxZ = vec1.clipZ(vec2, m_aabb.max.z, clipMaxZ) && containsZ(clipMaxZ);

	// the collided side of our AABB
	HitResult::eHitSide collType = HitResult::NOHIT;

	// the preferred vector for our collision
	Vec3* pVec = nullptr;
	if (bClipMinX)
		pVec = &clipMinX, collType = HitResult::MINX;

	if (bClipMaxX)
	{
		if (!pVec || clipMaxX.distanceToSqr(vec1) < pVec->distanceToSqr(vec1))
			pVec = &clipMaxX, collType = HitResult::MAXX;
	}

	if (bClipMinY)
	{
		if (!pVec || clipMinY.distanceToSqr(vec1) < pVec->distanceToSqr(vec1))
			pVec = &clipMinY, collType = HitResult::MINY;
	}

	if (bClipMaxY)
	{
		if (!pVec || clipMaxY.distanceToSqr(vec1) < pVec->distanceToSqr(vec1))
			pVec = &clipMaxY, collType = HitResult::MAXY;
	}

	if (bClipMinZ)
	{
		if (!pVec || clipMinZ.distanceToSqr(vec1) < pVec->distanceToSqr(vec1))
			pVec = &clipMinZ, collType = HitResult::MINZ;
	}

	if (bClipMaxZ)
	{
		if (!pVec || clipMaxZ.distanceToSqr(vec1) < pVec->distanceToSqr(vec1))
			pVec = &clipMaxZ, collType = HitResult::MAXZ;
	}

	if (!pVec)
	{
		// return a nothing burger
		return HitResult();
	}

	return HitResult(x, y, z, collType, *pVec + Vec3(float(x), float(y), float(z)));
}

int Tile::getSignal(LevelSource* pLevel, int x, int y, int z)
{
	return 0;
}

int Tile::getSignal(LevelSource* pLevel, int x, int y, int z, int dir)
{
	return 0;
}

int Tile::getDirectSignal(Level* pLevel, int x, int y, int z, int dir)
{
	return 0;
}

void Tile::triggerEvent(Level* pLevel, int x, int y, int z, int a, int b)
{

}

void Tile::entityInside(Level* pLevel, int x, int y, int z, Entity* pEnt)
{

}

void Tile::handleEntityInside(Level* pLevel, int x, int y, int z, Entity* pEnt, Vec3& vec)
{

}

float Tile::getDestroyProgress(Player* player)
{
	if (m_hardness < 0.0f)
		return 0.0f;

	if (!player->canDestroy(this))
		return (1.0f - m_hardness) / 100.0f;

	return player->getDestroySpeed() / m_hardness / 30.0f;
}

void Tile::spawnResources(Level* pLevel, int x, int y, int z, int i)
{
	return spawnResources(pLevel, x, y, z, i, 1.0f);
}

void Tile::spawnResources(Level* pLevel, int x, int y, int z, int data, float fChance)
{
	if (pLevel->m_bIsMultiplayer)
		return;

	int count = getResourceCount(&pLevel->m_random);
	for (int i = 0; i < count; i++)
	{
		if (pLevel->m_random.nextFloat() > fChance)
			continue;

		int id = getResource(data, &pLevel->m_random);
		if (id <= 0)
			continue;

		float xo = (pLevel->m_random.nextFloat() * 0.7f) + (1.0f - 0.7f) * 0.5f;
		float yo = (pLevel->m_random.nextFloat() * 0.7f) + (1.0f - 0.7f) * 0.5f;
		float zo = (pLevel->m_random.nextFloat() * 0.7f) + (1.0f - 0.7f) * 0.5f;

		ItemInstance inst(id, 1, getSpawnResourcesAuxValue(data));
		ItemEntity* pEntity = new ItemEntity(pLevel, float(x) + xo, float(y) + yo, float(z) + zo, &inst);
		pEntity->field_E4 = 10;

		pLevel->addEntity(pEntity);
	}
}

int Tile::spawnBurnResources(Level*, float, float, float)
{
	return 0;
}

float Tile::getExplosionResistance(Entity* entity)
{
	return m_blastResistance / 5.0f;
}

void Tile::wasExploded(Level* pLevel, int x, int y, int z)
{

}

int Tile::use(Level* pLevel, int x, int y, int z, Player* player)
{
	return 0;
}

void Tile::stepOn(Level* pLevel, int x, int y, int z, Entity* entity)
{

}

void Tile::setPlacedOnFace(Level* pLevel, int x, int y, int z, int dir)
{

}

void Tile::setPlacedBy(Level* pLevel, int x, int y, int z, Mob* mob)
{

}

void Tile::prepareRender(Level* pLevel, int x, int y, int z)
{

}

void Tile::attack(Level* pLevel, int x, int y, int z, Player* player)
{

}

void Tile::playerDestroy(Level* level, Player* player, int x, int y, int z, int i)
{
	spawnResources(level, x, y, z, i);
}

Tile::SoundType
	Tile::SOUND_NORMAL("stone",  1.0f, 1.0f),
	Tile::SOUND_WOOD  ("wood",   1.0f, 1.0f),
	Tile::SOUND_GRAVEL("gravel", 1.0f, 1.0f),
	Tile::SOUND_GRASS ("grass",  0.5f, 1.0f),
	Tile::SOUND_STONE ("stone",  1.0f, 1.0f),
	Tile::SOUND_METAL ("stone",  1.0f, 1.5f),
	Tile::SOUND_GLASS ("stone",  1.0f, 1.0f),
	Tile::SOUND_CLOTH ("cloth",  1.0f, 1.0f),
	Tile::SOUND_SAND  ("sand",   1.0f, 1.0f),
	Tile::SOUND_SILENT("",       1.0f, 1.0f);

// @TODO: Refactor this so that Tile::fire is already a FireTile* etc
Tile
	*Tile::sand,
	*Tile::sandStone,
	*Tile::stoneBrick,
	*Tile::redBrick,
	*Tile::wood,
	*Tile::glass,
	*Tile::calmWater,
	*Tile::calmLava,
	*Tile::gravel,
	*Tile::rock,
	*Tile::unbreakable,
	*Tile::dirt,
	*Tile::grass,
	*Tile::ice,
	*Tile::clay,
	*Tile::farmland,
	*Tile::stoneSlab,
	*Tile::stoneSlabHalf,
	*Tile::cloth,
	*Tile::cloth_00,
	*Tile::cloth_10,
	*Tile::cloth_20,
	*Tile::cloth_30,
	*Tile::cloth_40,
	*Tile::cloth_50,
	*Tile::cloth_60,
	*Tile::cloth_70,
	*Tile::cloth_01,
	*Tile::cloth_11,
	*Tile::cloth_21,
	*Tile::cloth_31,
	*Tile::cloth_41,
	*Tile::cloth_51,
	*Tile::cloth_61,
	*Tile::flower,
	*Tile::rose,
	*Tile::mushroom1,
	*Tile::mushroom2,
	*Tile::topSnow,
	*Tile::treeTrunk,
	*Tile::leaves,
	*Tile::emeraldOre, //! actually diamond ore
	*Tile::redStoneOre,
	*Tile::redStoneOre_lit,
	*Tile::goldOre,
	*Tile::ironOre,
	*Tile::coalOre,
	*Tile::lapisOre,
	*Tile::reeds,
	*Tile::ladder,
	*Tile::obsidian,
	*Tile::tnt,
	*Tile::torch,
	*Tile::water,
	*Tile::lava,
	*Tile::fire,
	*Tile::invisible_bedrock,
	*Tile::goldBlock,
	*Tile::ironBlock,
	*Tile::emeraldBlock, //! actually diamond block
	*Tile::stairs_wood,
	*Tile::stairs_stone,
	*Tile::door_wood,
	*Tile::door_iron,
	*Tile::info_updateGame1,
	*Tile::info_updateGame2,
	*Tile::sapling,
	*Tile::sponge,
	*Tile::lapisBlock,
	*Tile::bookshelf,
	*Tile::mossStone,
	*Tile::cryingObsidian,
	*Tile::rocketLauncher;
