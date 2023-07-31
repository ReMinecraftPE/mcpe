/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "Vec3.hpp"
#include "BiomeSource.hpp"
#include "Level.hpp"
#include "ChunkSource.hpp"

class Level; // if included from Level.hpp

class Dimension
{
public:
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
	Level* m_pLevel = nullptr;
	BiomeSource* m_pBiomeSource = nullptr;
	bool field_C = false;
	bool field_D = false;
	bool field_E = false;
	float field_10[16];
	int field_50 = 0;
	float m_sunriseColor[4];
};

