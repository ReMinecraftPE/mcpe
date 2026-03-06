/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "Dimension.hpp"
#include "GameMods.hpp"
#include "world/entity/MobSpawner.hpp"
#include "world/level/levelgen/biome/BiomeSource.hpp"
#include "world/level/levelgen/chunk/ChunkSource.hpp"
#include "world/level/TileSource.hpp"
#include "Level.hpp"

#include "world/level/levelgen/chunk/MainChunkSource.hpp"
#include "world/level/levelgen/chunk/NetworkChunkSource.hpp"
#include "world/level/levelgen/chunk/WorldLimitChunkSource.hpp"
#include "world/level/levelgen/chunk/RandomLevelSource.hpp"

#define C_TIMEOFDAY_SCALE_JAVA 24000
#define C_TIMEOFDAY_SCALE_POCKET 14400
#define C_TIMEOFDAY_SCALE C_TIMEOFDAY_SCALE_JAVA

Dimension::Dimension(Level& level, DimensionId dimensionId)
	: m_level(level)
	, m_biomeSource(nullptr) // TODO: nuke
	, m_bFoggy(false)
	, m_bUltraWarm(false)
	, m_hasCeiling(false)
	, m_dimensionId(dimensionId)
	, m_chunkSource(nullptr)
	, m_tileSource(nullptr)
{
	m_level.addListener(this);
}

Dimension::~Dimension()
{
	m_level.removeListener(*this);
	delete m_biomeSource;
}

ChunkSource* Dimension::_getGenerator(GeneratorType type)
{
	switch (type)
	{
	case GENERATOR_OVERWORLD:
	case GENERATOR_OVERWORLD_LIMITED:
		return new RandomLevelSource(&m_level, this, m_level.getSeed());
	default:
		return nullptr;
	}
}

void Dimension::init()
{
	GeneratorType generatorType = GENERATOR_OVERWORLD_LIMITED;

	// incase we initialise again
	delete m_chunkSource;

	if (!m_level.m_bIsClientSide)
	{
		ChunkSource* generator = _getGenerator(generatorType);
		m_chunkSource = new MainChunkSource(std::unique_ptr<ChunkSource>(generator));
	}
	else
	{
		m_chunkSource = new NetworkChunkSource(&m_level, this);
	}

	if (generatorType == GENERATOR_OVERWORLD_LIMITED)
	{
		m_chunkSource = new WorldLimitChunkSource(std::unique_ptr<ChunkSource>(m_chunkSource), m_level.getLevelData()->getLimitedWorldOrigin());
	}

	delete m_tileSource;
	m_tileSource = new TileSource(m_level, *this, *m_chunkSource, true, false);
	updateLightRamp();
}

void Dimension::tick()
{
	m_level.m_pMobSpawner->tick(*getTileSource(), m_level.m_difficulty > 0, true);
	//getChunkSource()->tick();

	for (EntityIdMap_t::iterator it = m_entityIdMap.begin(); it != m_entityIdMap.end(); it++)
	{
		Entity* pEnt = it->second;

		if (!pEnt->m_bRemoved)
		{
			pEnt->m_posPrev = pEnt->m_pos;
			pEnt->m_oRot = pEnt->m_rot;

			if (pEnt->m_bInAChunk)
				pEnt->tick();
		}
		else if (!pEnt->isPlayer() || pEnt->m_bForceRemove)
		{
			m_entityIdMap.erase(it);

			m_level.entityRemoved(pEnt);
			delete pEnt;
		}
	}
}

Dimension* Dimension::createNew(DimensionId type, Level& level)
{
	switch (type)
	{
	case DIMENSION_OVERWORLD:
		return new NormalDimension(level);
	default:
		return nullptr;
	}
}

Color Dimension::getFogColor(float a, float b) const
{
	float x1 = cosf(a * M_PI * 2.0f);
	float x2 = x1 * 2 + 0.5f;

	if (x2 < 0.0f)
		return Color(0.045176f, 0.050824f, 0.09f, 1.0f);

	Color color;
	color.a = 1.0f;
	if (x2 <= 1.0f)
	{
		float p = (x2 * 0.94f) + 0.06f;
		color.r = p * 0.75294f;
		color.g = p * 0.84706f;
		color.b = (x2 * 0.91f) + 0.09f;
	}
	else
	{
		color.r = 0.75294f;
		color.g = 0.84706f;
		color.b = 1.0f;
	}

	return color;
}

Color Dimension::getFogColor(float f) const
{
	return getFogColor(getTimeOfDay(f), f);
}

Color Dimension::getSkyColor(const Entity& entity, float f) const
{
	Color color;
	color.a = 1.0f;

	float fTODCosAng = Mth::cos(getSunAngle(f));

	color.b = 2 * fTODCosAng + 0.5f;
	if (color.b < 0.0f)
		color.b = 0.0f;
	if (color.b > 1.0f)
		color.b = 1.0f;

	// @NOTE: Unused result. In JE, it tries to get the biome that the player is standing in.
	//Mth::floor(entity.m_pos.x);
	//Mth::floor(entity.m_pos.z);

	color.r = color.b * 0.6f;
	color.g = color.r;

	return color;
}

float Dimension::getSunAngle(float f) const
{
	return 2 * float(M_PI) * getTimeOfDay(f);
}

Color Dimension::getSunriseColor(float a, float b) const
{
	Color sunriseColor;

	float radial = 0.4f;
	float dot = Mth::cos(a * M_PI * 2.0f) - 0.125f; // * 2.0f + 0.5f;
	float center = -0.0f;

	if (dot >= center - radial && dot <= center + radial)
	{
		float norm = (dot - center) / radial * 0.5f + 0.5f;
		float alpha = 1.0f - (1.0f - Mth::sin(norm * M_PI)) * 0.99f;
		
		sunriseColor.r = norm * 0.3f + 0.7f;
		sunriseColor.g = norm * norm * 0.7f + 0.2f;
		sunriseColor.b = norm * norm * 0.0f + 0.2f;
		sunriseColor.a = alpha * alpha;
	}

	return sunriseColor;
}

float Dimension::getTimeOfDay(int32_t l, float f) const
{
	int i = int(l % C_TIMEOFDAY_SCALE);
	float f1 = (float(i) + f) / float(C_TIMEOFDAY_SCALE) - 0.25f;
	if (f1 <  0.0f)
		f1++;
	if (f1 >  1.0f)
		f1--;

	// @NOTE: At this point, if the day/night cycle isn't running,
	// f1's value should be -0.25

	float f2 = f1;
	//@NOTE: Meant to use Mth::cos?
	f1 = 1.0f - (cosf(float(M_PI) * f1) + 1.0f) / 2.f;
	f1 = f2 + (f1 - f2) / 3.0f;
	return f1;
}

float Dimension::getTimeOfDay(float f) const
{
	return getTimeOfDay(m_level.getTime(), f);
}

void Dimension::updateLightRamp()
{
	constexpr float var1 = 0.05f;

	for (int i = 0; i <= Brightness::MAX; i++)
	{
		float f1 = float(i) / Brightness::MAX;
		float f2 = 1.0f - f1;

#ifdef ENH_USE_JAVA_LIGHT_RAMP
		m_brightnessRamp[i] = ((1.0f - f2) / (f2 * 3.0f + 1.0f)) * (1.0f - var1) + var1;
#else
		// 0.1.0
		// @NOTE: Adjusted calculation causes full bright tiles to render at 80% brightness.
		// This was probably done so that highlighted tiles don't have their brightness blown up and the texture doesn't look weird.
		m_brightnessRamp[i] = ((1.0f - ((i * -0.0625f) + 1.0f)) / ((((i * -0.0625f) + 1.0f) * 3.0f) + 1.0f)) * 0.95f + 0.05f;

		// 0.12.1
		/*float v6 = f1 / ((f2 * 3.0f) + 1.0f);
		m_brightnessRamp[i] = Mth::clamp(v6, 0.0f, 1.0f);*/
#endif
	}
}

bool Dimension::mayRespawn() const
{
	return true;
}

bool Dimension::isValidSpawn(const TilePos& pos) const
{
	TileID tile = m_tileSource->getTopSolidBlock(pos, true);
	if (tile == Tile::invisible_bedrock->m_ID)
		return false;

#ifndef ORIGINAL_CODE
	if (tile == TILE_AIR)
		return false;
#endif
	
	return Tile::tiles[tile]->isSolidRender();
}

bool Dimension::isNaturalDimension() const
{
	return true;
}

Color Dimension::getCloudColor(float f) const
{
	Color color = Color::WHITE;

	float fTODCosAng = Mth::cos(getSunAngle(f));

	float mult = 2 * fTODCosAng + 0.5f;
	if (mult < 0.0f)
		mult = 0.0f;
	if (mult > 1.0f)
		mult = 1.0f;

	color.r = mult * 0.9f + 0.1f;
	color.g = color.r;
	color.b = mult * 0.85f + 0.15f;

	return color;
}

float Dimension::getStarBrightness(float f) const
{
	float ca = Mth::cos(getSunAngle(f));
	float cb = 1.0f - (0.75f + 2 * ca);

	if (cb < 0.0f)
		cb = 0.0f;
	if (cb > 1.0f)
		cb = 1.0f;

	return cb * cb * 0.5f;
}

Entity* Dimension::getEntity(Entity::ID id) const
{
	EntityIdMap_t::const_iterator iter = m_entityIdMap.find(id);
	if (iter != m_entityIdMap.end())
		return iter->second;

	return nullptr;
}

bool Dimension::hasEntity(Entity& entity) const
{
	return m_entityIdMap.find(entity.hashCode()) != m_entityIdMap.end();
}

void Dimension::addEntity(Entity& entity)
{
	m_entityIdMap[entity.hashCode()] = &entity;
}

bool Dimension::removeEntity(Entity& entity)
{
	EntityIdMap_t::iterator iter = m_entityIdMap.find(entity.hashCode());
	if (iter != m_entityIdMap.end())
	{
		m_entityIdMap.erase(iter);
		return true;
	}

	return false;
}

Dimension::EntityIdMap_t& Dimension::getEntityIdMap()
{
	return m_entityIdMap;
}

const Dimension::EntityIdMap_t& Dimension::getEntityIdMapConst() const
{
	return m_entityIdMap;
}

NormalDimension::NormalDimension(Level& level)
	: Dimension(level, DIMENSION_OVERWORLD)
{
}

void NormalDimension::init()
{
	Dimension::init();
}

std::string NormalDimension::getName() const
{
	return "Overworld";
}
