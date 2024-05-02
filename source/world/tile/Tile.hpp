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

class Level;
class Entity;
class Mob;
class Player;
class LiquidTile;

// TODO: split out tiles into their own header files.

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
	virtual void updateShape(const LevelSource*, int, int, int);
	virtual void updateDefaultShape();
	virtual void addLights(Level*, int, int, int);
	virtual float getBrightness(const LevelSource*, int, int, int) const;
	virtual bool shouldRenderFace(const LevelSource*, int, int, int, int) const;
	virtual int getTexture(int) const;
	virtual int getTexture(int, int) const;
	virtual int getTexture(const LevelSource*, int, int, int, int) const;
	virtual AABB* getAABB(const Level*, int, int, int);
	virtual void addAABBs(const Level*, int, int, int, const AABB*, std::vector<AABB>&);
	virtual AABB getTileAABB(const Level*, int, int, int);
	virtual bool isSolidRender() const;
	virtual bool mayPick() const;
	virtual bool mayPick(int, bool) const;
	virtual bool mayPlace(const Level*, int, int, int) const;
	virtual int getTickDelay() const;
	virtual void tick(Level*, int, int, int, Random*);
	virtual void animateTick(Level*, int, int, int, Random*);
	virtual void destroy(Level*, int, int, int, int dir);
	virtual void neighborChanged(Level*, int, int, int, int);
	virtual void onPlace(Level*, int, int, int);
	virtual void onRemove(Level*, int, int, int);
	virtual int getResource(int, Random*) const;
	virtual int getResourceCount(Random*) const;
	virtual float getDestroyProgress(Player*) const;
	virtual void spawnResources(Level*, int, int, int, int);
	virtual void spawnResources(Level*, int, int, int, int, float);
	virtual int spawnBurnResources(Level*, float, float, float);
	virtual float getExplosionResistance(Entity*) const;
	virtual HitResult clip(const Level*, int, int, int, Vec3, Vec3);
	virtual void wasExploded(Level*, int, int, int);
	virtual int getRenderLayer() const;
	virtual int use(Level*, int, int, int, Player*);
	virtual void stepOn(Level*, int, int, int, Entity*);
	virtual void setPlacedOnFace(Level*, int, int, int, int);
	virtual void setPlacedBy(Level*, int, int, int, Mob*);
	virtual void prepareRender(Level*, int, int, int);
	virtual void attack(Level*, int, int, int, Player*);
	virtual void handleEntityInside(Level*, int, int, int, const Entity*, Vec3&);
	virtual int getColor(const LevelSource*, int, int, int) const;
	virtual bool isSignalSource() const;
	virtual int getSignal(const LevelSource*, int, int, int) const;
	virtual int getSignal(const LevelSource*, int, int, int, int) const;
	virtual int getDirectSignal(const Level*, int, int, int, int) const;
	virtual void entityInside(Level*, int, int, int, Entity*) const;
	virtual void playerDestroy(Level*, Player*, int, int, int, int);
	virtual bool canSurvive(const Level*, int, int, int) const;
	virtual std::string getName() const;
	virtual std::string getDescriptionId() const;
	virtual Tile* setDescriptionId(std::string const&);
	virtual void triggerEvent(Level*, int, int, int, int, int);
	virtual Tile* setSoundType(Tile::SoundType const&);
	virtual Tile* setLightBlock(int);
	virtual Tile* setLightEmission(float);
	virtual Tile* setExplodeable(float);
	virtual Tile* setDestroyTime(float);
	virtual Tile* setTicking(bool);
	virtual int getSpawnResourcesAuxValue(int) const;

private:
	void _init();
	void _init(int ID, Material* pMaterial, int texture = 1);
	Tile() { _init(); } // consider making public?
public: // functions
	Tile(int ID, Material* pMaterial) { _init(ID, pMaterial); }
	Tile(int ID, int texture, Material* pMaterial) { _init(ID, pMaterial, texture); }

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
		* snow,
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
		* rocketLauncher;

public:
	int m_TextureFrame;
	int m_ID;
	AABB m_aabb;
	const SoundType* m_pSound;
	float field_28;
	Material* m_pMaterial;
	float field_30;
	float m_hardness;
	float m_blastResistance;
	AABB m_aabbReturned;
	std::string m_descriptionID;
};
