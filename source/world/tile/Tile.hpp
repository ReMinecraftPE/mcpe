/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include <string>
#include <vector>

#include "common/Random.hpp"
#include "world/phys/AABB.hpp"
#include "world/level/storage/LevelSource.hpp"
#include "world/level/Material.hpp"
#include "world/entity/Entity.hpp"
#include "world/level/levelgen/chunk/LevelChunk.hpp"
#include "world/Facing.hpp"
#include "world/level/TilePos.hpp"
#include "world/phys/Vec3.hpp"
#include "world/phys/HitResult.hpp"

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
		float volume, pitch;

		SoundType(const std::string& name, float volume, float pitch) : m_name(name), volume(volume), pitch(pitch) {}
	};

public: // virtual functions
	virtual ~Tile();
	virtual bool isCubeShaped() const;
	virtual int getRenderShape() const;
	virtual Tile* setShape(float, float, float, float, float, float);
	virtual void updateShape(const LevelSource*, const TilePos& pos);
	virtual void updateDefaultShape();
	virtual void addLights(Level*, const TilePos& pos);
	virtual float getBrightness(const LevelSource*, const TilePos& pos) const;
	virtual bool shouldRenderFace(const LevelSource*, const TilePos& pos, Facing::Name face) const;
	virtual int getTexture(Facing::Name face) const;
	virtual int getTexture(Facing::Name face, TileData data) const;
	virtual int getTexture(const LevelSource*, const TilePos& pos, Facing::Name face) const;
	virtual AABB* getAABB(const Level*, const TilePos& pos);
	virtual void addAABBs(const Level*, const TilePos& pos, const AABB*, std::vector<AABB>&);
	virtual AABB getTileAABB(const Level*, const TilePos& pos);
	virtual bool isSolidRender() const;
	virtual bool mayPick() const;
	virtual bool mayPick(TileData, bool) const;
	virtual bool mayPlace(const Level*, const TilePos& pos) const;
	virtual int getTickDelay() const;
	virtual void tick(Level*, const TilePos& pos, Random*);
	virtual void animateTick(Level*, const TilePos& pos, Random*);
	virtual void destroy(Level*, const TilePos& pos, TileData data);
	virtual void neighborChanged(Level*, const TilePos& pos, TileID tile);
	virtual void onPlace(Level*, const TilePos& pos);
	virtual void onRemove(Level*, const TilePos& pos);
	virtual int getResource(TileData, Random*) const;
	virtual int getResourceCount(Random*) const;
	virtual float getDestroyProgress(Player*) const;
	virtual void spawnResources(Level*, const TilePos& pos, TileData data);
	virtual void spawnResources(Level*, const TilePos& pos, TileData data, float);
	virtual int spawnBurnResources(Level*, float, float, float);
	virtual float getExplosionResistance(Entity*) const;
	virtual HitResult clip(const Level*, const TilePos& pos, Vec3, Vec3);
	virtual void wasExploded(Level*, const TilePos& pos);
	virtual int getRenderLayer() const;
	virtual bool use(Level*, const TilePos& pos, Player*);
	virtual void stepOn(Level*, const TilePos& pos, Entity*);
	virtual void setPlacedOnFace(Level*, const TilePos& pos, Facing::Name face);
	virtual void setPlacedBy(Level*, const TilePos& pos, Mob*);
	virtual void prepareRender(Level*, const TilePos& pos);
	virtual void attack(Level*, const TilePos& pos, Player*);
	virtual void handleEntityInside(Level*, const TilePos& pos, const Entity*, Vec3&);
	virtual int getColor(const LevelSource*, const TilePos& pos) const;
	virtual bool isSignalSource() const;
	virtual int getSignal(const LevelSource*, const TilePos& pos) const;
	virtual int getSignal(const LevelSource*, const TilePos& pos, Facing::Name face) const;
	virtual int getDirectSignal(const Level*, const TilePos& pos, Facing::Name face) const;
	virtual void entityInside(Level*, const TilePos& pos, Entity*) const;
	virtual void playerDestroy(Level*, Player*, const TilePos& pos, TileData data);
	virtual void playerWillDestroy(Player*, const TilePos& pos, TileData data);
	virtual bool canSurvive(const Level*, const TilePos& pos) const;
	virtual std::string getName() const;
	virtual std::string getDescriptionId() const;
	virtual Tile* setDescriptionId(std::string const&);
	virtual void triggerEvent(Level*, const TilePos& pos, int, int);
	virtual Tile* setSoundType(Tile::SoundType const&);
	virtual Tile* setLightBlock(int);
	virtual Tile* setLightEmission(float);
	virtual Tile* setExplodeable(float);
	virtual Tile* setDestroyTime(float);
	virtual Tile* setTicking(bool);
	virtual int getSpawnResourcesAuxValue(int) const;

private:
	void _init();
	void _init(TileID ID, Material* pMaterial, int texture = 1);
	Tile() { _init(); } // consider making public?
public: // functions
	Tile(TileID ID, Material* pMaterial) { _init(ID, pMaterial); }
	Tile(TileID ID, int texture, Material* pMaterial) { _init(ID, pMaterial, texture); }

	Tile* init();

	bool containsX(const Vec3&);
	bool containsY(const Vec3&);
	bool containsZ(const Vec3&);

public: // static functions
	static void initTiles();
	static void teardownTiles();
	static TileID TransformToValidBlockId(TileID tileId, TilePos pos);
	static TileID TransformToValidBlockId(TileID tileId);

public: // static variables
	static std::string TILE_DESCRIPTION_PREFIX;
	static const SoundType
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
		* snow,
		* clay,
		* farmland,
		* stoneSlab,
		* stoneSlabHalf,
		* cloth,
		* flower,
		* rose,
		* mushroom1,
		* mushroom2,
		* topSnow,
		* treeTrunk,
		* leaves,
		* leaves_carried,
		* info_reserved6,
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
		* door_iron,
		* info_updateGame1,
		* info_updateGame2,
		// custom additions here
		* sapling,
		* sponge,
		* lapisBlock,
		* bookshelf,
		* mossStone,
		* cryingObsidian,
		* rocketLauncher,
		* cactus,
		* tallGrass,
		* deadBush,
		* pumpkin,
		* pumpkinLantern,
		* netherrack,
		* soulSand,
		* glowstone,
		* web,
		* fence;

public:
	int m_TextureFrame;
	TileID m_ID;
	AABB m_aabb;
	const SoundType* m_pSound;
	float field_28;
	Material* m_pMaterial;
	float m_friction;
	float m_hardness;
	float m_blastResistance;
	AABB m_aabbReturned;
	std::string m_descriptionID;
};
