/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "client/common/Vec3.hpp"
#include "world/level/levelgen/biome/BiomeSource.hpp"
#include "world/level/levelgen/chunk/ChunkSource.hpp"
#include "Level.hpp"

class Level; // if included from Level.hpp

class Dimension
{
public:
	Dimension();
	virtual ~Dimension();
	static Dimension* getNew(int type);

	virtual Vec3 getFogColor(float, float);
	virtual void init();
	virtual bool mayRespawn();
	virtual bool isValidSpawn(int x, int z);

	float* getSunriseColor(float, float);
	float getTimeOfDay(TLong, float);
	void init(Level* pLevel);
	void updateLightRamp();

	ChunkSource* createRandomLevelSource();

public:
	Level* m_pLevel;
	BiomeSource* m_pBiomeSource;
	bool field_C;
	bool field_D;
	bool field_E;
	float field_10[16];
	int field_50;
	float m_sunriseColor[4];
};

