/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include <string>
#include <unordered_map>
#include "world/level/LevelListener.hpp"
#include "common/math/Color.hpp"
#include "world/entity/Entity.hpp"
#include "world/level/Brightness.hpp"

class Level;
class ChunkSource;
class TileSource;
class BiomeSource;

enum DimensionId
{
	DIMENSION_OVERWORLD,
	DIMENSION_NETHER,
	DIMENSION_THE_END,
	DIMENSIONS_COUNT,
	DIMENSION_UNKNOWN
};

enum GeneratorType
{
	GENERATOR_OVERWORLD_LIMITED,
	GENERATOR_OVERWORLD,
	GENERATOR_FLAT,
	GENERATOR_NETHER,
	GENERATOR_THE_END
};

class Dimension : public LevelListener
{
public:
	typedef std::unordered_map<Entity::ID, Entity*> EntityIdMap_t;

protected:
	Level& m_level;
	BiomeSource* m_biomeSource;
	bool m_bFoggy;
	bool m_bUltraWarm;
	bool m_hasCeiling;
	float m_brightnessRamp[Brightness::MAX + 1];
	DimensionId m_dimensionId;
	ChunkSource* m_chunkSource;
	TileSource* m_tileSource;
	EntityIdMap_t m_entityIdMap;

public:
	Dimension(Level& level, DimensionId dimensionId);
	virtual ~Dimension();

public:
	virtual void init();
	virtual void tick();
	virtual Color getFogColor(float, float) const;
	virtual bool isNaturalDimension() const;
	virtual bool mayRespawn() const;
	virtual bool isValidSpawn(const TilePos& pos) const;
	virtual void updateLightRamp();
	virtual std::string getName() const = 0;

	bool isDay() const { return true; /*m_skyDarken <= 3;*/ } // @TODO: find this var
	bool isFoggy() const { return m_bFoggy; }
	Color getFogColor(float f) const;
	Color getSkyColor(const Entity& entity, float f) const;
	float getSunAngle(float f) const;
	Color getSunriseColor(float, float) const;
	float getTimeOfDay(int32_t, float) const;
	float getTimeOfDay(float f) const;
	Color getCloudColor(float f) const;
	float getStarBrightness(float f) const;

	Entity* getEntity(Entity::ID id) const;
	bool hasEntity(Entity& entity) const;
	void addEntity(Entity& entity);
	bool removeEntity(Entity& entity);
	EntityIdMap_t& getEntityIdMap();
	const EntityIdMap_t& getEntityIdMapConst() const;

	DimensionId getId() const
	{
		return m_dimensionId;
	}

	bool isWarm() const
	{
		return m_bUltraWarm;
	}

	bool hasCeiling() const
	{
		return m_hasCeiling;
	}

	float getBrightnessRamp(Brightness_t brightness) const
	{
		return m_brightnessRamp[brightness];
	}

	ChunkSource* getChunkSource()
	{
		return m_chunkSource;
	}

	TileSource* getTileSource()
	{
		return m_tileSource;
	}

protected:
	ChunkSource* _getGenerator(GeneratorType type);

public:
	static Dimension* createNew(DimensionId type, Level& level);
};

class NormalDimension : public Dimension
{
public:
	NormalDimension(Level& level);

public:
	void init() override;
	std::string getName() const override;
};
