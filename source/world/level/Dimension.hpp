/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "world/phys/Vec3.hpp"

class Level; // if included from Level.hpp
class ChunkSource;
class BiomeSource;

enum DimensionId
{
	DIMENSION_NORMAL,
	DIMENSION_HELL
};

class Dimension
{
public:
	Dimension();
	virtual ~Dimension();
	static Dimension* createNew(DimensionId type);

	virtual Vec3 getFogColor(float, float);
	virtual void init();
	virtual bool mayRespawn();
	virtual bool isValidSpawn(const TilePos& pos);

	float* getSunriseColor(float, float);
	float getTimeOfDay(int32_t, float);
	void init(Level* pLevel);
	void updateLightRamp();

	ChunkSource* createRandomLevelSource();

public:
	Level* m_pLevel;
	BiomeSource* m_pBiomeSource;
	bool m_bFoggy;
	bool field_D;
	bool field_E;
	float m_brightnessRamp[16];
	int field_50;
	float m_sunriseColor[4];
};

