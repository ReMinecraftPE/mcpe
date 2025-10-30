/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "Dimension.hpp"
#include "GameMods.hpp"
#include "world/level/levelgen/chunk/TestChunkSource.hpp"
#include "world/level/levelgen/chunk/RandomLevelSource.hpp"
#include "world/level/levelgen/chunk/ChunkCache.hpp"

#define C_TIMEOFDAY_SCALE_JAVA 24000
#define C_TIMEOFDAY_SCALE_POCKET 14400
#define C_TIMEOFDAY_SCALE C_TIMEOFDAY_SCALE_JAVA

Dimension* Dimension::getNew(int type)
{
	switch (type)
	{
	case 0:
		return new Dimension;
	/*case -1:
		return new HellDimension;*/
	default: // type not supported
		return nullptr;
	}
}

Vec3 Dimension::getFogColor(float a, float b)
{
	float x1 = cosf(a * M_PI * 2.0f);
	float x2 = x1 * 2 + 0.5f;

	if (x2 < 0.0f)
		return Vec3(0.045176f, 0.050824f, 0.09f);

	Vec3 v;
	v.z = 1;

	if (x2 <= 1.0f)
	{
		float p = (x2 * 0.94f) + 0.06f;
		v.x = p * 0.75294f;
		v.y = p * 0.84706f;
		v.z = (x2 * 0.91f) + 0.09f;
	}
	else
	{
		v.x = 0.75294f;
		v.y = 0.84706f;
	}

	return v;
}

float* Dimension::getSunriseColor(float a, float b)
{
	float radial = 0.4f;
	float dot = Mth::cos(a * M_PI * 2.0f) - 0.125f; // * 2.0f + 0.5f;
	float center = -0.0f;

	if (dot >= center - radial && dot <= center + radial)
	{
		float norm = (dot - center) / radial * 0.5f + 0.5f;
		float alpha = 1.0f - (1.0f - Mth::sin(norm * M_PI)) * 0.99f;
		
		m_sunriseColor[0] = norm * 0.3f + 0.7f;
		m_sunriseColor[1] = norm * norm * 0.7f + 0.2f;
		m_sunriseColor[2] = norm * norm * 0.0f + 0.2f;
		m_sunriseColor[3] = alpha * alpha;

		return m_sunriseColor;
	}
	return nullptr;
/*
	
	float x1 = Mth::cos(a * M_PI * 2.0f); //@BUG: Meant to use Mth::cos?
	if (x1 < -0.4f || x1 > 0.4f)
		return nullptr;

	float x2 = x1 / 0.4f * 0.5f + 0.5f;
	float x3 = 1.0f - Mth::sin(x2);

	m_sunriseColor[0] = x2 * 0.3f + 0.7f;
	m_sunriseColor[1] = (x2 * x2) * 0.7f + 0.2f;
	m_sunriseColor[2] = (x2 * x2) * 0.0f + 0.2f; //@BUG: useless multiplication by 0?
	m_sunriseColor[3] = ((x3 * -0.99f) + 1.0f) * ((x3 * -0.99f) + 1.0f);

	return m_sunriseColor;
	*/
}

float Dimension::getTimeOfDay(int32_t l, float f)
{
#ifndef ENH_RUN_DAY_NIGHT_CYCLE
	//@QUIRK: This is a constant.
	l = 0;
	f = 0;
#endif

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

void Dimension::updateLightRamp()
{
	for (int i = 0; i < 16; i++)
	{
#ifdef ENH_USE_JAVA_LIGHT_RAMP
		float f1 = 1.0f - float(i) / 15.0f;
		field_10[i] = ((1.0f - f1) / (f1 * 3.0f + 1.0f)) * (1.0f - 0.1f) + 0.1f;
#else
		// @NOTE: Adjusted calculation causes full bright tiles to render at 80% brightness.
		// This was probably done so that highlighted tiles don't have their brightness blown up and the texture doesn't look weird.
		field_10[i] = ((1.0f - ((i * -0.0625f) + 1.0f)) / ((((i * -0.0625f) + 1.0f) * 3.0f) + 1.0f)) * 0.95f + 0.05f;
#endif
	}
}

void Dimension::init()
{
	m_pBiomeSource = new BiomeSource(m_pLevel);
}

void Dimension::init(Level* pLevel)
{
	m_pLevel = pLevel;
	init();
	updateLightRamp();
}

Dimension::Dimension()
{
	m_pLevel = nullptr;
	m_pBiomeSource = nullptr;
	m_bFoggy = false;
	field_D = false;
	field_E = false;
	field_50 = 0;
}

Dimension::~Dimension()
{
	if (m_pBiomeSource)
		delete m_pBiomeSource;
}

bool Dimension::mayRespawn()
{
	return true;
}

ChunkSource* Dimension::createRandomLevelSource()
{
#ifdef MOD_USE_FLAT_WORLD
	return new TestChunkSource(m_pLevel);
#else
	return new RandomLevelSource(m_pLevel, m_pLevel->getSeed(), m_pLevel->getLevelData()->getGeneratorVersion());
#endif
}

bool Dimension::isValidSpawn(const TilePos& pos)
{
	TileID tile = m_pLevel->getTopTile(pos);
	if (tile == Tile::invisible_bedrock->m_ID)
		return false;

#ifndef ORIGINAL_CODE
	if (tile == 0)
		return false;
#endif
	
	return Tile::tiles[tile]->isSolidRender();
}
