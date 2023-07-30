/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include <string>
#include <vector>

#include "Material.hpp"
#include "Random.hpp"
#include "AABB.hpp"
#include "LevelSource.hpp"
#include "Entity.hpp"

class Level;
class Entity;
class Mob;
class Player;
class LiquidTile;

class Tile
{
public: // structs
	struct SoundType
	{
		std::string m_name;
		float field_18, field_1C;

		SoundType(const std::string& a, float b, float c) : m_name(a), field_18(b), field_1C(c) {}
	};

public: // virtual functions
	virtual ~Tile();
	virtual bool isCubeShaped();
	virtual int getRenderShape();
	virtual Tile* setShape(float, float, float, float, float, float);
	virtual void updateShape(LevelSource*, int, int, int);
	virtual void updateDefaultShape();
	virtual void addLights(Level*, int, int, int);
	virtual float getBrightness(LevelSource*, int, int, int);
	virtual bool shouldRenderFace(LevelSource*, int, int, int, int);
	virtual int getTexture(int);
	virtual int getTexture(int, int);
	virtual int getTexture(LevelSource*, int, int, int, int);
	virtual AABB* getAABB(Level*, int, int, int);
	virtual void addAABBs(Level*, int, int, int, const AABB*, std::vector<AABB>&);
	virtual AABB getTileAABB(Level*, int, int, int);
	virtual bool isSolidRender();
	virtual bool mayPick();
	virtual bool mayPick(int, bool);
	virtual bool mayPlace(Level*, int, int, int);
	virtual int getTickDelay();
	virtual void tick(Level*, int, int, int, Random*);
	virtual void animateTick(Level*, int, int, int, Random*);
	virtual void destroy(Level*, int, int, int, int dir);
	virtual void neighborChanged(Level*, int, int, int, int);
	virtual void onPlace(Level*, int, int, int);
	virtual void onRemove(Level*, int, int, int);
	virtual int getResource(int, Random*);
	virtual int getResourceCount(Random*);
	virtual float getDestroyProgress(Player*);
	virtual void spawnResources(Level*, int, int, int, int);
	virtual void spawnResources(Level*, int, int, int, int, float);
	virtual int spawnBurnResources(Level*, float, float, float);
	virtual float getExplosionResistance(Entity*);
	virtual HitResult clip(Level*, int, int, int, Vec3, Vec3);
	virtual void wasExploded(Level*, int, int, int);
	virtual int getRenderLayer();
	virtual int use(Level*, int, int, int, Player*);
	virtual void stepOn(Level*, int, int, int, Entity*);
	virtual void setPlacedOnFace(Level*, int, int, int, int);
	virtual void setPlacedBy(Level*, int, int, int, Mob*);
	virtual void prepareRender(Level*, int, int, int);
	virtual void attack(Level*, int, int, int, Player*);
	virtual void handleEntityInside(Level*, int, int, int, Entity*, Vec3&);
	virtual int getColor(LevelSource*, int, int, int);
	virtual bool isSignalSource();
	virtual int getSignal(LevelSource*, int, int, int);
	virtual int getSignal(LevelSource*, int, int, int, int);
	virtual int getDirectSignal(Level*, int, int, int, int);
	virtual void entityInside(Level*, int, int, int, Entity*);
	virtual void playerDestroy(Level*, Player*, int, int, int, int);
	virtual bool canSurvive(Level*, int, int, int);
	virtual std::string getName();
	virtual std::string getDescriptionId();
	virtual Tile* setDescriptionId(std::string const&);
	virtual void triggerEvent(Level*, int, int, int, int, int);
	virtual Tile* setSoundType(Tile::SoundType const&);
	virtual Tile* setLightBlock(int);
	virtual Tile* setLightEmission(float);
	virtual Tile* setExplodeable(float);
	virtual Tile* setDestroyTime(float);
	virtual Tile* setTicking(bool);
	virtual int getSpawnResourcesAuxValue(int);

public: // functions
	Tile(int ID, Material*);
	Tile(int ID, int texture, Material*);

	Tile* init();

	bool containsX(const Vec3&);
	bool containsY(const Vec3&);
	bool containsZ(const Vec3&);

public: // static functions
	static void initTiles();
	static void teardownTiles();

public: // static variables
	static std::string TILE_DESCRIPTION_PREFIX;
	static SoundType
		SOUND_NORMAL,
		SOUND_WOOD,
		SOUND_GRAVEL,
		SOUND_GRASS,
		SOUND_STONE,
		SOUND_METAL,
		SOUND_GLASS,
		SOUND_CLOTH,
		SOUND_SAND,
		SOUND_SILENT;
	static Tile* tiles        [C_MAX_TILES];
	static int   lightBlock   [C_MAX_TILES];
	static int   lightEmission[C_MAX_TILES];
	static bool  shouldTick   [C_MAX_TILES];
	static bool  solid        [C_MAX_TILES];
	static bool  translucent  [C_MAX_TILES];
	static bool  isEntityTile [C_MAX_TILES];

	// TODO
	static Tile
		* sand,
		* sandStone,
		* stoneBrick,
		* redBrick,
		* wood,
		* glass,
		* calmWater,
		* calmLava,
		* gravel,
		* rock,
		* unbreakable,
		* dirt,
		* grass,
		* ice,
		* clay,
		* farmland,
		* stoneSlab,
		* stoneSlabHalf,
		* cloth,
		* cloth_00,
		* cloth_10,
		* cloth_20,
		* cloth_30,
		* cloth_40,
		* cloth_50,
		* cloth_60,
		* cloth_70,
		* cloth_01,
		* cloth_11,
		* cloth_21,
		* cloth_31,
		* cloth_41,
		* cloth_51,
		* cloth_61,
		* flower,
		* rose,
		* mushroom1,
		* mushroom2,
		* topSnow,
		* treeTrunk,
		* leaves,
		* emeraldOre, //! actually diamond ore
		* redStoneOre,
		* redStoneOre_lit,
		* goldOre,
		* ironOre,
		* coalOre,
		* lapisOre,
		* reeds,
		* ladder,
		* obsidian,
		* tnt,
		* torch,
		* water,
		* lava,
		* fire,
		* invisible_bedrock,
		* goldBlock,
		* ironBlock,
		* emeraldBlock, //! actually diamond block
		* stairs_wood,
		* stairs_stone,
		* door_wood,
		* door_iron;

public:
	int m_TextureFrame = 1;
	int m_ID;
	AABB m_aabb;
	const SoundType* m_pSound = nullptr;
	float field_28 = 1.0f;
	Material* m_pMaterial;
	float field_30 = 0.6f;
	float m_hardness = 0.0f;
	float m_blastResistance = 0.0f;
	AABB m_aabbReturned;
	std::string m_descriptionID = "";
};

class SandTile : public Tile
{
public:
	SandTile(int ID, int texture, Material*);
	
	int getTickDelay() override;
	void tick(Level* level, int x, int y, int z, Random* random) override;
	void neighborChanged(Level* level, int x, int y, int z, int dir) override;
	void onPlace(Level* level, int x, int y, int z) override;

	void checkSlide(Level* level, int x, int y, int z);
	static bool isFree(Level* level, int x, int y, int z);

	static bool instaFall;
};

class SandStoneTile : public Tile
{
public:
	SandStoneTile(int ID, int texture, Material*);
	
	int getTexture(int side) override;
};

class HalfTransparentTile : public Tile
{
public:
	HalfTransparentTile(int ID, int texture, Material*);

	virtual bool isSolidRender() override;
	virtual bool shouldRenderFace(LevelSource*, int, int, int, int) override;

public:
	bool field_6C = false;
};

class GlassTile : public HalfTransparentTile
{
public:
	GlassTile(int ID, int texture, Material*);
	
	int getResourceCount(Random*) override;
};

class GravelTile : public SandTile
{
public:
	GravelTile(int ID, int texture, Material*);
	
	int getResource(int, Random*) override;
};

class StoneTile : public Tile
{
public:
	StoneTile(int ID, int texture, Material*);
	
	int getResource(int, Random*) override;
};

class DirtTile : public Tile
{
public:
	DirtTile(int ID, int texture, Material*);

	//@NOTE: No difference
};

class GrassTile : public Tile
{
public:
	GrassTile(int ID, Material*);

	int getResource(int, Random*) override;
	int getColor(LevelSource*, int x, int y, int z) override;
	int getTexture(LevelSource*, int x, int y, int z, int dir) override;
	void tick(Level*, int x, int y, int z, Random*) override;
};

class IceTile : public HalfTransparentTile
{
public:
	IceTile(int ID, int texture, Material*);

	int getRenderLayer() override;
	int getResourceCount(Random*) override;
	void onRemove(Level*, int x, int y, int z) override;
	bool shouldRenderFace(LevelSource*, int x, int y, int z, int dir) override;
	void tick(Level*, int x, int y, int z, Random*) override;
};

class ClayTile : public Tile
{
public:
	ClayTile(int ID, int texture, Material*);

	int getResource(int, Random*) override;
	int getResourceCount(Random*) override;
};

class FarmTile : public Tile
{
public:
	FarmTile(int ID, Material*);

	AABB* getAABB(Level*, int x, int y, int z) override;
	int getResource(int, Random*) override;
	int getTexture(int dir, int data) override;
	bool isCubeShaped() override;
	bool isSolidRender() override;
	void neighborChanged(Level*, int x, int y, int z, int dir) override;
	void stepOn(Level* level, int x, int y, int z, Entity* pEnt) override;
	void tick(Level* level, int x, int y, int z, Random* random) override;

	bool isNearWater(Level* level, int x, int y, int z);
};

class StoneSlabTile : public Tile
{
public:
	StoneSlabTile(int ID, bool bFull);

	bool isSolidRender() override;
	bool isCubeShaped() override;
	int getResource(int, Random*) override;
	int getResourceCount(Random*) override;
	int getSpawnResourcesAuxValue(int) override;
	int getTexture(int dir) override;
	int getTexture(int dir, int data) override;
	void onPlace(Level*, int x, int y, int z) override;
	bool shouldRenderFace(LevelSource*, int x, int y, int z, int dir) override;

	bool m_bFull = false;
};

class ClothTile : public Tile
{
public:
	ClothTile(int id, int type);
	int getTexture(int dir) override;
	int getTexture(int dir, int data) override;
	int getSpawnResourcesAuxValue(int val) override;

	uint8_t field_6C;
};

class Bush : public Tile
{
public:
	Bush(int id, int texture);

	bool canSurvive(Level*, int x, int y, int z) override;
	AABB* getAABB(Level*, int x, int y, int z) override;
	int getRenderShape() override;
	bool isCubeShaped() override;
	bool isSolidRender() override;
	bool mayPlace(Level*, int x, int y, int z) override;
	void tick(Level*, int x, int y, int z, Random*) override;
	void neighborChanged(Level*, int x, int y, int z, int dir) override;

	void checkAlive(Level*, int x, int y, int z);
};


class TopSnowTile : public Tile
{
public:
	TopSnowTile(int id, int texture, Material* pMtl);

	AABB* getAABB(Level*, int x, int y, int z) override;
	bool isCubeShaped() override;
	bool isSolidRender() override;
	int getResource(int, Random*) override;
	int getResourceCount(Random*) override;
	bool mayPlace(Level*, int x, int y, int z) override;
	void neighborChanged(Level*, int x, int y, int z, int d) override;
	bool shouldRenderFace(LevelSource*, int x, int y, int z, int i) override;
	void tick(Level*, int x, int y, int z, Random*) override;

	bool checkCanSurvive(Level*, int x, int y, int z);
};

class TreeTile : public Tile
{
public:
	TreeTile(int ID);

	int getResource(int, Random*) override;
	int getResourceCount(Random*) override;
	int getSpawnResourcesAuxValue(int) override;
	int getTexture(int dir, int data) override;
	void onRemove(Level*, int x, int y, int z) override;
};

class TransparentTile : public Tile
{
public:
	TransparentTile(int ID, int texture, Material*, bool bTransparent);

	virtual bool isSolidRender() override;
	virtual bool shouldRenderFace(LevelSource*, int, int, int, int) override;

public:
	bool m_bTransparent;
};

class LeafTile : public TransparentTile
{
public:
	LeafTile(int id);
	~LeafTile();

	int getColor(LevelSource*, int, int, int) override;
	int getTexture(int dir, int data) override;
	bool isSolidRender() override;
	void onRemove(Level*, int x, int y, int z) override;
	void stepOn(Level*, int x, int y, int z, Entity*) override;
	void tick(Level*, int x, int y, int z, Random*) override;

	void die(Level*, int, int, int);

	int* field_70 = nullptr;
	int field_74 = 0;
};

class OreTile : public Tile
{
public:
	OreTile(int id, int texture);

	int getResource(int, Random*) override;
	int getResourceCount(Random*) override;
	int getSpawnResourcesAuxValue(int) override;
};

class RedStoneOreTile : public Tile
{
public:
	RedStoneOreTile(int id, int texture, bool bLit);

	int getResource(int, Random*) override;
	int getResourceCount(Random*) override;
	int getSpawnResourcesAuxValue(int) override;
	int getTickDelay() override;
	void animateTick(Level*, int x, int y, int z, Random*) override;
	void tick(Level*, int x, int y, int z, Random*) override;
	void attack(Level*, int x, int y, int z, Player*) override;
	int use(Level*, int x, int y, int z, Player*) override;
	void stepOn(Level*, int x, int y, int z, Entity*) override;

	int poofParticles(Level*, int x, int y, int z);
	void interact(Level*, int x, int y, int z);

	bool m_bLit = false;
};

class ReedTile : public Tile
{
public:
	ReedTile(int id);

	bool canSurvive(Level*, int x, int y, int z) override;
	AABB* getAABB(Level*, int x, int y, int z) override;
	int getRenderShape() override;
	bool isCubeShaped() override;
	bool isSolidRender() override;
	bool mayPlace(Level*, int x, int y, int z) override;
	void tick(Level*, int x, int y, int z, Random*) override;
	void neighborChanged(Level*, int x, int y, int z, int dir) override;
	int getResource(int, Random*) override;

	void checkAlive(Level*, int x, int y, int z);
};

class InvisibleTile : public Tile
{
public:
	InvisibleTile(int ID, int texture, Material*);
	int getRenderShape() override;
};

class LiquidTile : public Tile
{
public:
	LiquidTile(int ID, Material*);

	virtual void onPlace(Level*, int x, int y, int z) override;
	virtual void neighborChanged(Level*, int x, int y, int z, int dir) override;
	virtual void tick(Level*, int x, int y, int z, Random* random) override;
	void animateTick(Level*, int x, int y, int z, Random* random) override;
	AABB* getAABB(Level*, int x, int y, int z) override;
	float getBrightness(LevelSource*, int x, int y, int z) override;
	int getRenderLayer() override;
	int getRenderShape() override;
	int getResource(int, Random*) override;
	int getResourceCount(Random*) override;
	int getTexture(int dir) override;
	int getTexture(int dir, int data) override;
	int getTickDelay() override;
	void handleEntityInside(Level*, int, int, int, Entity*, Vec3&) override;
	bool isCubeShaped() override;
	bool isSolidRender() override;
	bool mayPick(int data, bool b) override;
	bool shouldRenderFace(LevelSource*, int x, int y, int z, int dir) override;

	void updateLiquid(Level*, int x, int y, int z);
	void fizz(Level*, int x, int y, int z);
	int getColor(LevelSource*, int x, int y, int z);
	int getDepth(Level*, int x, int y, int z);
	int getRenderedDepth(LevelSource*, int x, int y, int z);
	Vec3 getFlow(LevelSource*, int x, int y, int z);

	static float getSlopeAngle(LevelSource*, int x, int y, int z, Material* pMtl);

	// @NOTE: This is inlined in minecraftcpp
	static float getWaterVolume(int data)
	{
		if (data >= 8)
			data = 0;

		return float(data + 1) / 9.0f;
	}

public:
	int field_6C = 0;
	bool field_70[4] = { 0 };
	int field_74 [4] = { 0 };
};

class LiquidTileDynamic : public LiquidTile
{
public:
	LiquidTileDynamic(int id, Material* pMtl);

	void onPlace(Level*, int x, int y, int z) override;
	void tick(Level*, int x, int y, int z, Random*) override;

	bool isWaterBlocking(Level*, int x, int y, int z);
	bool canSpreadTo(Level*, int x, int y, int z);
	int getSlopeDistance(Level*, int, int, int, int, int);
	bool* getSpread(Level*, int x, int y, int z);
	void setStatic(Level*, int x, int y, int z);
	void trySpreadTo(Level*, int x, int y, int z, int a);
	int getSmallestDepth(Level*, int x, int y, int z, int oldDepth);
};

class LiquidTileStatic : public LiquidTile
{
public:
	LiquidTileStatic(int id, Material* pMtl);

	void neighborChanged(Level* level, int x, int y, int z, int dir) override;
	void tick(Level* level, int x, int y, int z, Random* random) override;

	bool isFlammable(Level*, int x, int y, int z);
	void setDynamic(Level*, int x, int y, int z);
};

class ObsidianTile : public Tile
{
public:
	ObsidianTile(int ID, int texture, Material*);

	int getResource(int data, Random*) override;
	int getResourceCount(Random*) override;
};

class MetalTile : public Tile
{
public:
	MetalTile(int ID, int texture, Material*);

	virtual int getTexture(int dir) override;
};

class StairTile : public Tile
{
public:
	StairTile(int ID, Tile* pParent);

	virtual void addAABBs(Level*, int x, int y, int z, const AABB*, std::vector<AABB>&) override;
	virtual bool isSolidRender() override;
	virtual bool isCubeShaped() override;
	virtual int getRenderShape() override;

	// Just overloads to forward to parent tile.
	virtual void addLights(Level*, int x, int y, int z) override;
	virtual void animateTick(Level*, int x, int y, int z, Random*) override;
	virtual void updateShape(LevelSource*, int, int, int) override;
	virtual float getBrightness(LevelSource*, int, int, int) override;
	virtual int getTexture(int) override;
	virtual int getTexture(int, int) override;
	virtual int getTexture(LevelSource*, int, int, int, int) override;
	virtual AABB getTileAABB(Level*, int, int, int) override;
	virtual bool mayPick() override;
	virtual bool mayPick(int, bool) override;
	virtual bool mayPlace(Level*, int, int, int) override;
	virtual int getTickDelay() override;
	virtual void tick(Level*, int, int, int, Random*) override;
	virtual void destroy(Level*, int, int, int, int dir) override;
	virtual void onPlace(Level*, int, int, int) override;
	virtual void onRemove(Level*, int, int, int) override;
	virtual int getResource(int, Random*) override;
	virtual int getResourceCount(Random*) override;
	virtual void spawnResources(Level*, int, int, int, int) override;
	virtual void spawnResources(Level*, int, int, int, int, float) override;
	virtual float getExplosionResistance(Entity*) override;
	virtual void wasExploded(Level*, int, int, int) override;
	virtual int getRenderLayer() override;
	virtual int use(Level*, int, int, int, Player*) override;
	virtual void stepOn(Level*, int, int, int, Entity*) override;
	virtual void setPlacedBy(Level*, int, int, int, Mob*) override;
	virtual void prepareRender(Level*, int, int, int) override;
	virtual void attack(Level*, int, int, int, Player*) override;
	virtual void handleEntityInside(Level*, int, int, int, Entity*, Vec3&) override;

public:
	Tile* m_pParent;
};

class TorchTile : public Tile
{
public:
	TorchTile(int ID, int texture, Material* pMtl);

	AABB* getAABB(Level*, int x, int y, int z) override;
	bool isSolidRender() override;
	bool isCubeShaped() override;
	int getRenderShape() override;
	void animateTick(Level*, int x, int y, int z, Random*) override;
	HitResult clip(Level*, int x, int y, int z, Vec3 a, Vec3 b) override;
	bool mayPlace(Level*, int, int, int) override;
	void neighborChanged(Level*, int, int, int, int) override;
	void onPlace(Level*, int, int, int) override;
	void setPlacedOnFace(Level*, int, int, int, int) override;
	void tick(Level*, int, int, int, Random*) override;

	bool checkCanSurvive(Level*, int x, int y, int z);
};

class FireTile : public Tile
{
public:
	FireTile(int ID, int texture);

	AABB* getAABB(Level*, int x, int y, int z);
	int getRenderShape() override;
	bool isCubeShaped() override;
	bool isSolidRender() override;
	int getResourceCount(Random*) override;
	int getTickDelay() override;
	bool mayPick() override;
	bool mayPlace(Level*, int x, int y, int z) override;
	void animateTick(Level*, int x, int y, int z, Random*) override;
	void neighborChanged(Level*, int x, int y, int z, int dir) override;
	void onPlace(Level*, int x, int y, int z) override;
	void tick(Level*, int x, int y, int z, Random*) override;

	void checkBurn(Level*, int x, int y, int z, int, Random*);
	int getFireOdds(Level*, int x, int y, int z);
	bool isValidFireLocation(Level*, int x, int y, int z);

	// @NOTE: This is inlined in V0.1.0 but not V0.7.1
	inline bool canBurn(LevelSource* level, int x, int y, int z)
	{
		return m_igniteOdds[level->getTile(x, y, z)] > 0;
	}

public:
	int m_igniteOdds[C_MAX_TILES];
	int m_burnOdds[C_MAX_TILES];
};

class LadderTile : public Tile
{
public:
	LadderTile(int ID, int texture);

	bool isCubeShaped() override;
	bool isSolidRender() override;
	int getRenderShape() override;
	int getResourceCount(Random* random) override;
	AABB* getAABB(Level*, int x, int y, int z);
	AABB getTileAABB(Level*, int x, int y, int z);
	void setPlacedOnFace(Level*, int x, int y, int z, int face) override;
	void neighborChanged(Level*, int, int, int, int) override;
	bool mayPlace(Level*, int, int, int) override;
};

class TntTile : public Tile
{
public:
	TntTile(int ID, int texture);

	int getResourceCount(Random*) override;
	int getTexture(int dir) override;
	void neighborChanged(Level*, int x, int y, int z, int d) override;
	void destroy(Level*, int x, int y, int z, int data) override;
	void wasExploded(Level*, int x, int y, int z) override;
};

class DoorTile : public Tile
{
public:
	DoorTile(int ID, Material*);

	void attack(Level*, int x, int y, int z, Player*) override;
	int use(Level*, int x, int y, int z, Player*) override;
	HitResult clip(Level*, int x, int y, int z, Vec3, Vec3) override;
	AABB* getAABB(Level*, int x, int y, int z) override;
	int getRenderShape() override;
	int getResource(int data, Random*) override;
	int getTexture(int dir, int data) override;
	AABB getTileAABB(Level*, int x, int y, int z) override;
	bool isCubeShaped() override;
	bool isSolidRender() override;
	bool mayPlace(Level*, int x, int y, int z) override;
	void updateShape(LevelSource*, int x, int y, int z) override;
	void neighborChanged(Level*, int x, int y, int z, int newTile) override;

	bool blocksLight();
	int getDir(int data);
	void setShape(int dir);
	void setOpen(Level*, int x, int y, int z, bool bOpen);

	// @NOTE: These are inlined.
	inline bool isOpen(int data)
	{
		return (data & 4) != 0;
	}
	inline bool isTop(int data)
	{
		return (data & 8) != 0;
	}
};
