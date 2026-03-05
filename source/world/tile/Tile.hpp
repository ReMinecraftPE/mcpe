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
#include "common/Utils.hpp"
#include "world/item/Tool.hpp"
#include "world/phys/AABB.hpp"
#include "world/level/Material.hpp"
#include "world/entity/Entity.hpp"
#include "world/Facing.hpp"
#include "world/level/TilePos.hpp"
#include "world/level/TileEvent.hpp"
#include "world/phys/Vec3.hpp"
#include "world/phys/HitResult.hpp"
#include "world/level/Brightness.hpp"

class Level;
class Entity;
class Mob;
class Player;
class LiquidTile;
class TileSource;

class Tile
{
public: // types
	enum RenderLayer
	{
		RENDER_LAYER_DOUBLE_SIDED,
		RENDER_LAYER_BLEND,
		RENDER_LAYER_OPAQUE,
		RENDER_LAYER_OPTIONAL_ALPHATEST,
		RENDER_LAYER_ALPHATEST,
		RENDER_LAYER_SEASONS_OPAQUE,
		RENDER_LAYER_SEASONS_OPTIONAL_ALPHATEST,
		RENDER_LAYER_ALPHATEST_SINGLE_SIDE,
		RENDER_LAYERS_MIN = RENDER_LAYER_DOUBLE_SIDED,
		RENDER_LAYERS_MAX = RENDER_LAYER_ALPHATEST_SINGLE_SIDE,
		RENDER_LAYERS_COUNT
	};
	struct SoundType
	{
		std::string m_name;
		float volume, pitch;

		SoundType(const std::string& name, float volume, float pitch) : m_name(name), volume(volume), pitch(pitch) {}
	};

public: // virtual functions
	virtual ~Tile();
	virtual bool isCubeShaped() const;
	virtual eRenderShape getRenderShape() const;
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Woverloaded-virtual"
	virtual Tile* setShape(float, float, float, float, float, float);
#pragma GCC diagnostic pop
	virtual void updateShape(TileSource*, const TilePos& pos);
	virtual void updateDefaultShape();
	virtual void addLights(TileSource*, const TilePos& pos);
	virtual float getBrightness(TileSource*, const TilePos& pos) const;
	virtual bool shouldRenderFace(TileSource*, const TilePos& pos, Facing::Name face) const;
	virtual int getTexture(Facing::Name face) const;
	virtual int getTexture(Facing::Name face, TileData data) const;
	virtual int getTexture(TileSource*, const TilePos& pos, Facing::Name face) const;
	virtual AABB* getAABB(TileSource*, const TilePos& pos);
	virtual void addAABBs(TileSource*, const TilePos& pos, const AABB*, std::vector<AABB>&);
	virtual AABB getTileAABB(TileSource*, const TilePos& pos);
	virtual bool isSolidRender() const;
	virtual bool mayPick() const;
	virtual bool mayPick(TileData, bool) const;
	virtual bool mayPlace(TileSource*, const TilePos& pos) const;
	virtual bool tryToPlace(TileSource*, const TilePos&, TileData);
	virtual int getTickDelay() const;
	virtual void tick(TileSource* source, const TilePos& pos, Random* random);
	virtual void animateTick(TileSource*, const TilePos& pos, Random*);
	virtual void destroy(TileSource*, const TilePos& pos, TileData data);
	virtual void neighborChanged(TileSource*, const TilePos& pos, TileID tile);
	virtual void onPlace(TileSource*, const TilePos& pos);
	virtual void onRemove(TileSource*, const TilePos& pos);
	virtual int getResource(TileData, Random*) const;
	virtual int getResourceCount(Random*) const;
	virtual float getDestroyProgress(Player*) const;
	virtual void spawnResources(TileSource*, const TilePos& pos, TileData data);
	virtual void spawnResources(TileSource*, const TilePos& pos, TileData data, float);
	virtual int spawnBurnResources(TileSource*, float, float, float);
	virtual float getExplosionResistance(Entity*) const;
	virtual HitResult clip(TileSource*, const TilePos& pos, Vec3, Vec3);
	virtual void wasExploded(TileSource*, const TilePos& pos);
	virtual RenderLayer getRenderLayer(TileSource*, const TilePos&) const;
	virtual bool use(const TilePos& pos, Player*);
	virtual void stepOn(TileSource*, const TilePos& pos, Entity*);
	virtual void setPlacedOnFace(TileSource*, const TilePos& pos, Facing::Name face);
	virtual void setPlacedBy(TileSource*, const TilePos& pos, Mob*);
	virtual void prepareRender(TileSource*, const TilePos& pos);
	virtual void attack(TileSource*, const TilePos& pos, Player*);
	virtual void handleEntityInside(TileSource*, const TilePos& pos, const Entity*, Vec3&);
	virtual int getColor(TileSource*, const TilePos& pos) const;
	virtual bool isSignalSource() const;
	virtual int getSignal(TileSource*, const TilePos& pos) const;
	virtual int getSignal(TileSource*, const TilePos& pos, Facing::Name face) const;
	virtual int getDirectSignal(TileSource*, const TilePos& pos, Facing::Name face) const;
	virtual void entityInside(TileSource*, const TilePos& pos, Entity*) const;
	virtual void playerDestroy(TileSource*, Player*, const TilePos& pos, TileData data);
	virtual void playerWillDestroy(Player*, const TilePos& pos, TileData data);
	virtual bool canSurvive(TileSource*, const TilePos& pos) const;
	virtual std::string getName() const;
	virtual std::string getDescriptionId() const;
	virtual Tile* setDescriptionId(std::string const&);
	virtual void triggerEvent(TileSource*, const TileEvent& event);
	virtual Tile* setSoundType(Tile::SoundType const&);
	virtual Tile* setLightBlock(int);
	virtual Tile* setLightEmission(float);
	virtual Tile* setExplodeable(float);
	virtual Tile* setDestroyTime(float);
	virtual Tile* setTicking(bool);
	virtual int getSpawnResourcesAuxValue(int) const;
	virtual bool isSeasonTinted() const;
	Tile* setToolTypes(unsigned int toolMask);
	Tile* setToolLevel(int toolLevel);
	Tile* setToolTypesAndLevel(unsigned int toolMask, int toolLevel = 0);

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

	bool isSolid() const
	{
		return solid[m_ID];
	}

	bool operator==(const Tile& other) const
	{
		return m_ID == other.m_ID;
	}

	bool operator==(TileID id) const
	{
		return m_ID == id;
	}

	bool operator!=(const Tile& other) const
	{
		return m_ID != other.m_ID;
	}

	bool operator!=(TileID id) const
	{
		return m_ID != id;
	}

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
	static Brightness_t   lightBlock   [C_MAX_TILES];
	static Brightness_t   lightEmission[C_MAX_TILES];
	static bool  shouldTick   [C_MAX_TILES];
	static bool  solid        [C_MAX_TILES];
	static bool  translucent  [C_MAX_TILES];
	static float translucency [C_MAX_TILES];
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
		* fence,
		* craftingTable,
		* crops;

public:
	int m_TextureFrame;
	TileID m_ID;
	AABB m_aabb;
	const SoundType* m_pSound;
	float m_gravity; // only affects particle gravity at the moment
	Material* m_pMaterial;
	float m_friction;
	float m_hardness;
	float m_blastResistance;
	unsigned int m_toolMask;
	int m_requiredToolLevel;
	AABB m_aabbReturned;
	std::string m_descriptionID;

protected:
	RenderLayer m_renderLayer;
};

class FullTile
{
private:
	Tile* _tileType;
public:
	TileData data;

private:
	void _init(Tile* tileType, TileData data)
	{
		this->_tileType = tileType;
		this->data = data;
	}

public:
	FullTile(TileID tileId, TileData data)
	{
		_init(Tile::tiles[tileId], data);
	}

	FullTile(Tile* tileType, TileData data)
	{
		_init(tileType, data);
	}

public:
	TileID getTypeId() const { return _tileType->m_ID; }
	Tile* getType() const { return _tileType; }

	bool operator==(const FullTile& other) const
	{
		return _tileType == other._tileType && data == other.data;
	}

	bool operator!=(const FullTile& other) const
	{
		return _tileType != other._tileType || data != other.data;
	}
};
