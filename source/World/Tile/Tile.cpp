/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "Level.hpp"
#include "Item.hpp"
#include "ItemEntity.hpp"

std::string Tile::TILE_DESCRIPTION_PREFIX = "tile.";

Tile* Tile::tiles        [C_MAX_TILES];
int   Tile::lightBlock   [C_MAX_TILES];
int   Tile::lightEmission[C_MAX_TILES];
bool  Tile::shouldTick   [C_MAX_TILES];
bool  Tile::solid        [C_MAX_TILES];
bool  Tile::translucent  [C_MAX_TILES];
bool  Tile::isEntityTile [C_MAX_TILES];


Tile::Tile(int ID, Material* pMaterial) :
	m_aabb(0, 0, 0, 1, 1, 1),
	m_aabbReturned(0, 0, 0, 1, 1, 1)
{
	m_ID = ID;
	m_pMaterial = pMaterial;
	m_pSound = &SOUND_NORMAL;

	if (tiles[m_ID])
		// @BUG: Printing &tiles[m_ID], but probably supposed to print tiles[m_ID]
		printf("Slot %d is already occupied by %p when adding %p\n", m_ID, &tiles[m_ID], this);
}

Tile::Tile(int ID, int texture, Material* pMaterial) :
	m_aabb(0, 0, 0, 1, 1, 1),
	m_aabbReturned(0, 0, 0, 1, 1, 1)
{
	m_TextureFrame = texture;
	m_ID = ID;
	m_pMaterial = pMaterial;
	m_pSound = &SOUND_NORMAL;

	if (tiles[m_ID])
		// @BUG: Printing &tiles[m_ID], but probably supposed to print tiles[m_ID]
		printf("Slot %d is already occupied by %p when adding %p\n", m_ID, &tiles[m_ID], this);
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
		out.emplace_back(*pTileAABB);
	}
}

bool Tile::shouldRenderFace(LevelSource* pSrc, int x, int y, int z, int dir)
{
	if (z == -1                  && dir == DIR_ZNEG) return false;
	if (z == C_MAX_CHUNKS_Z * 16 && dir == DIR_ZPOS) return false;
	if (x == -1                  && dir == DIR_XNEG) return false;
	if (x == C_MAX_CHUNKS_X * 16 && dir == DIR_XPOS) return false;
	if (y == -1                  && dir == DIR_YNEG) return false;

	if (dir == DIR_YNEG && m_aabb.min.y > 0.0f) return true;
	if (dir == DIR_YPOS && m_aabb.max.y < 1.0f) return true;
	if (dir == DIR_ZNEG && m_aabb.min.z > 0.0f) return true;
	if (dir == DIR_ZPOS && m_aabb.max.z < 1.0f) return true;
	if (dir == DIR_XNEG && m_aabb.min.x > 0.0f) return true;
	if (dir == DIR_XPOS && m_aabb.max.x < 1.0f) return true;

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
	return v.y >= m_aabb.min.y && v.y <= m_aabb.max.y && v.z >= m_aabb.min.z && v.z <= m_aabb.max.z;
}

bool Tile::containsY(const Vec3& v)
{
	return v.x >= m_aabb.min.x && v.x <= m_aabb.max.x && v.z >= m_aabb.min.z && v.z <= m_aabb.max.z;
}

bool Tile::containsZ(const Vec3& v)
{
	return v.x >= m_aabb.min.x && v.x <= m_aabb.max.x && v.y >= m_aabb.min.z && v.y <= m_aabb.max.y;
}

HitResult Tile::clip(Level* level, int x, int y, int z, Vec3 vec1, Vec3 vec2)
{
	// @TODO: clean up this stuff

	float negX; // s14
	float aabbMinX; // r2
	float v13; // s11
	float negY; // s13
	float negZ; // s12
	float v16; // s10
	float v17; // s9
	float v18; // s14
	float v19; // s13
	float v20; // s12
	int v21; // r0
	int v22; // r12
	int v23; // r5
	int v24; // r6
	Vec3* v25; // r3
	float v26; // s14
	float v27; // s13
	float v28; // s15
	HitResult::eHitSide v29; // r2
	float v30; // s13
	float v31; // s14
	float v32; // s15
	bool v34; // r0
	int v35; // [sp+18h] [bp-60h]
	int v36; // [sp+1Ch] [bp-5Ch]
	int v37; // [sp+20h] [bp-58h]
	int v38; // [sp+24h] [bp-54h]
	int v39; // [sp+28h] [bp-50h]
	int v40; // [sp+2Ch] [bp-4Ch]
	int v41; // [sp+30h] [bp-48h]
	int v42; // [sp+34h] [bp-44h]
	Vec3 v43; // [sp+3Ch] [bp-3Ch] BYREF
	Vec3 v44; // [sp+48h] [bp-30h] BYREF
	Vec3 v45; // [sp+54h] [bp-24h] BYREF
	Vec3 v46; // [sp+60h] [bp-18h] BYREF
	Vec3 v47; // [sp+6Ch] [bp-Ch] BYREF
	Vec3 v48; // [sp+78h] [bp+0h] BYREF
	Vec3 v49; // [sp+84h] [bp+Ch] BYREF

	HitResult hr;

	this->updateShape(level, x, y, z);
	negX = (float)-x;
	aabbMinX = this->m_aabb.min.x;
	v13 = negX + vec1.x;
	v49.x = 0.0;
	v49.y = 0.0;
	v49.z = 0.0;
	v48.x = 0.0;
	v48.y = 0.0;
	v48.z = 0.0;
	v47.x = 0.0;
	v47.y = 0.0;
	v47.z = 0.0;
	v46.x = 0.0;
	v46.y = 0.0;
	v46.z = 0.0;
	v45.x = 0.0;
	v45.y = 0.0;
	v45.z = 0.0;
	v44.x = 0.0;
	negY = (float)-y;
	v44.y = 0.0;
	v44.z = 0.0;
	negZ = (float)-z;
	v16 = negY + vec1.y;
	v17 = negZ + vec1.z;
	if ((float)(negX + vec1.x) == 0.0)
		v13 = 0.0;
	vec1.x = v13;
	v18 = negX + vec2.x;
	v19 = negY + vec2.y;
	v20 = negZ + vec2.z;
	if (v16 == 0.0)
		v16 = 0.0;
	vec1.y = v16;
	if (v17 == 0.0)
		v17 = 0.0;
	vec1.z = v17;
	if (v18 == 0.0)
		v18 = 0.0;
	vec2.x = v18;
	if (v19 == 0.0)
		v19 = 0.0;
	if (v20 == 0.0)
		v20 = 0.0;
	vec2.y = v19;
	vec2.z = v20;
	v35 = vec1.clipX(vec2, aabbMinX, v49);
	v42 = vec1.clipX(vec2, this->m_aabb.max.x, v48);
	v36 = vec1.clipY(vec2, this->m_aabb.min.y, v47);
	v41 = v36;
	v37 = vec1.clipY(vec2, this->m_aabb.max.y, v46);
	v40 = v37;
	v38 = vec1.clipZ(vec2, this->m_aabb.min.z, v45);
	v39 = v38;
	v21 = vec1.clipZ(vec2, this->m_aabb.max.z, v44);
	v22 = v42;
	v23 = v21;
	v24 = v21;
	if (!v35 || (v34 = this->containsX(v49), v22 = v42, !v34))
		v35 = 0;
	if (!v22 || !this->containsX(v48))
		v42 = 0;
	if (!v36 || !this->containsY(v47))
		v41 = 0;
	if (!v37 || !this->containsY(v46))
		v40 = 0;
	if (!v38 || !this->containsZ(v45))
	{
		v39 = 0;
		if (v23)
			goto LABEL_59;
	LABEL_23:
		v24 = 0;
		goto LABEL_24;
	}
	if (!v23)
		goto LABEL_23;
LABEL_59:
	if (!this->containsZ(v44))
		v24 = 0;
LABEL_24:
	if (v35)
		v25 = &v49;
	else
		v25 = 0;
	if (v42
		&& (!v25
			|| (float)((float)((float)((float)(v48.y - vec1.y) * (float)(v48.y - vec1.y))
				+ (float)((float)(v48.x - vec1.x) * (float)(v48.x - vec1.x)))
				+ (float)((float)(v48.z - vec1.z) * (float)(v48.z - vec1.z))) < (float)((float)((float)((float)(v25->y - vec1.y) * (float)(v25->y - vec1.y))
					+ (float)((float)(v25->x - vec1.x) * (float)(v25->x - vec1.x)))
					+ (float)((float)(v25->z - vec1.z)
						* (float)(v25->z - vec1.z)))))
	{
		v25 = &v48;
	}
	if (v41
		&& (!v25
			|| (float)((float)((float)((float)(v47.y - vec1.y) * (float)(v47.y - vec1.y))
				+ (float)((float)(v47.x - vec1.x) * (float)(v47.x - vec1.x)))
				+ (float)((float)(v47.z - vec1.z) * (float)(v47.z - vec1.z))) < (float)((float)((float)((float)(v25->y - vec1.y) * (float)(v25->y - vec1.y))
					+ (float)((float)(v25->x - vec1.x) * (float)(v25->x - vec1.x)))
					+ (float)((float)(v25->z - vec1.z)
						* (float)(v25->z - vec1.z)))))
	{
		v25 = &v47;
	}
	if (v40
		&& (!v25
			|| (float)((float)((float)((float)(v46.y - vec1.y) * (float)(v46.y - vec1.y))
				+ (float)((float)(v46.x - vec1.x) * (float)(v46.x - vec1.x)))
				+ (float)((float)(v46.z - vec1.z) * (float)(v46.z - vec1.z))) < (float)((float)((float)((float)(v25->y - vec1.y) * (float)(v25->y - vec1.y))
					+ (float)((float)(v25->x - vec1.x) * (float)(v25->x - vec1.x)))
					+ (float)((float)(v25->z - vec1.z)
						* (float)(v25->z - vec1.z)))))
	{
		v25 = &v46;
	}
	if (v39
		&& (!v25
			|| (float)((float)((float)((float)(v45.y - vec1.y) * (float)(v45.y - vec1.y))
				+ (float)((float)(v45.x - vec1.x) * (float)(v45.x - vec1.x)))
				+ (float)((float)(v45.z - vec1.z) * (float)(v45.z - vec1.z))) < (float)((float)((float)((float)(v25->y - vec1.y) * (float)(v25->y - vec1.y))
					+ (float)((float)(v25->x - vec1.x) * (float)(v25->x - vec1.x)))
					+ (float)((float)(v25->z - vec1.z)
						* (float)(v25->z - vec1.z)))))
	{
		v25 = &v45;
		if (v24)
			goto LABEL_40;
	LABEL_62:
		if (!v25)
		{
			hr.m_hitPos.x = 0.0;
			hr.m_hitPos.y = 0.0;
			hr.m_hitPos.z = 0.0;
			hr.m_hitType = HitResult::NONE;
			return hr;
		}
		v27 = v25->x;
		v26 = v25->y;
		v28 = v25->z;
		goto LABEL_64;
	}
	if (!v24)
		goto LABEL_62;
LABEL_40:
	if (!v25
		|| (v26 = v25->y,
			v27 = v25->x,
			v28 = v25->z,
			(float)((float)((float)((float)(v44.y - vec1.y) * (float)(v44.y - vec1.y))
				+ (float)((float)(v44.x - vec1.x) * (float)(v44.x - vec1.x)))
				+ (float)((float)(v44.z - vec1.z) * (float)(v44.z - vec1.z))) < (float)((float)((float)((float)(v26 - vec1.y) * (float)(v26 - vec1.y))
					+ (float)((float)(v25->x - vec1.x) * (float)(v25->x - vec1.x)))
					+ (float)((float)(v28 - vec1.z)
						* (float)(v28 - vec1.z)))))
	{
		v25 = &v44;
		v27 = v44.x;
		v26 = v44.y;
		v28 = v44.z;
		goto LABEL_43;
	}
LABEL_64:
	if (v25 == &v49)
	{
		v29 = HitResult::MINX;
		if (v25 == &v46)
			goto LABEL_66;
		goto LABEL_48;
	}
LABEL_43:
	if (v25 != &v48)
	{
		if (v25 == &v47)
		{
			v29 = HitResult::MINY;
			goto LABEL_49;
		}
		v29 = HitResult::NOHIT;
		if (v25 != &v46)
			goto LABEL_48;
	LABEL_66:
		v29 = HitResult::MAXY;
		goto LABEL_51;
	}
	v29 = HitResult::MAXX;
LABEL_48:
	if (v25 != &v45)
	{
	LABEL_49:
		if (v25 == &v44)
			v29 = HitResult::MAXZ;
		goto LABEL_51;
	}
	v29 = HitResult::MINZ;
LABEL_51:
	v30 = (float)x + v27;
	v31 = (float)y + v26;
	v32 = (float)z + v28;
	if (v30 == 0.0)
		v30 = 0.0;
	v43.x = v30;
	if (v31 == 0.0)
		v31 = 0.0;
	if (v32 == 0.0)
		v32 = 0.0;
	v43.y = v31;
	v43.z = v32;
	return HitResult(x, y, z, v29, v43);
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
#ifdef TEST_DROPPED_ITEMS
	if (pLevel->field_11)
		return;

	int count = getResourceCount(&pLevel->field_38);
	for (int i = 0; i < count; i++)
	{
		if (pLevel->field_38.nextFloat() > fChance)
			continue;

		int id = getResource(data, &pLevel->field_38);
		if (id <= 0)
			continue;

		float xo = (pLevel->field_38.nextFloat() * 0.7f) + (1.0f - 0.7f) * 0.5f;
		float yo = (pLevel->field_38.nextFloat() * 0.7f) + (1.0f - 0.7f) * 0.5f;
		float zo = (pLevel->field_38.nextFloat() * 0.7f) + (1.0f - 0.7f) * 0.5f;

		ItemInstance inst(id, 1, getSpawnResourcesAuxValue(data));
		ItemEntity* pEntity = new ItemEntity(pLevel, float(x) + xo, float(y) + yo, float(z) + zo, &inst);
		pEntity->field_E4 = 10;

		pLevel->addEntity(pEntity);
	}
#endif
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
	*Tile::door_iron;
