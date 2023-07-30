/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include <string>

#include "Feature.hpp"
#include "PerlinNoise.hpp"
#include "Utils.hpp"

class Biome
{
public: // Static Functions
	static Biome* getBiome(float hum, float temp);
	static void initBiomes();

public: // Virtual functions
	virtual float adjustDepth(float depth);
	virtual float adjustScale(float scale);
	virtual int getSkyColor(float x);
	virtual Feature* getTreeFeature(Random*);

public: // Instance Functions
	Biome();
	virtual ~Biome();
	Biome* setColor(int color);
	Biome* setLeafColor(int color);
	Biome* setName(const std::string& name);
	Biome* setSnowCovered();

public: // Static Variables
	static Biome
		* rainForest,
		* swampland,
		* seasonalForest,
		* forest,
		* savanna,
		* shrubland,
		* taiga,
		* desert,
		* plains,
		* iceDesert,
		* tundra;
	static Biome* map[4096];

private: // Private Functions
	static Biome* _getBiome(float hum, float temp);
	static void recalc();

public: // Instance Variables
	std::string m_name = "";
	int m_Color = 0;
	TileID field_20 = 0;
	TileID field_21 = 0;
	int m_LeafColor = 0;
};

class RainforestBiome : public Biome
{
public:
	Feature* getTreeFeature(Random*) override;
};

class SwampBiome : public Biome
{
};

class ForestBiome : public Biome
{
public:
	Feature* getTreeFeature(Random*) override;
};

class TaigaBiome : public Biome
{
public:
	Feature* getTreeFeature(Random*) override;
};

class FlatBiome : public Biome
{
};
