/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "Biome.hpp"
#include "Tile.hpp"

Biome
* Biome::rainForest,
* Biome::swampland,
* Biome::seasonalForest,
* Biome::forest,
* Biome::savanna,
* Biome::shrubland,
* Biome::taiga,
* Biome::desert,
* Biome::plains,
* Biome::iceDesert,
* Biome::tundra;

Biome* Biome::map[4096];

Biome* Biome::_getBiome(float hum, float temp)
{
	if (temp < 0.1)
		return tundra;

	float ht = hum * temp;
	if (ht < 0.2f)
	{
		if (temp >= 0.5f)
		{
			if (temp >= 0.95f)
				return desert;
			return savanna;
		}
		return tundra;
	}

	if (ht > 0.5f && ht < 0.7f)
		return swampland;

	if (temp < 0.5f)
		return taiga;

	if (temp >= 0.97f)
	{
		if (ht < 0.45f)
			return plains;
		
		if (ht < 0.9f)
			return seasonalForest;
		
		return rainForest;
	}

	if (temp >= 0.35f)
		return forest;
	
	return shrubland;
}

float Biome::adjustDepth(float f)
{
	return f;
}

float Biome::adjustScale(float f)
{
	return f;
}

int Biome::getSkyColor(float x)
{
	return 0x80808080;
}

Biome* Biome::getBiome(float hum, float temp)
{
	int i = int(63.0f * hum);
	int j = int(63.0f * temp);

	return map[i + j * 64];
}

void Biome::recalc()
{
	for (int i = 0; i < 64; i++)
	{
		for (int j = 0; j < 64; j++)
		{
			map[i + j * 64] = _getBiome(float(i) / 63, float(j) / 63);
		}
	}

	desert->field_20 = desert->field_21 = Tile::sand->m_ID;
	iceDesert->field_20 = iceDesert->field_21 = Tile::sand->m_ID;
}

Biome::Biome()
{
	m_name = "";
	field_20 = Tile::grass->m_ID;
	field_21 = Tile::dirt->m_ID;
}

Biome::~Biome()
{
}

Feature* Biome::getTreeFeature(Random* pRandom)
{
	pRandom->genrand_int32(); //! unused result

	return new TreeFeature;
}

Biome* Biome::setSnowCovered()
{
	return this;
}

Biome* Biome::setColor(int color)
{
	m_Color = color;
	return this;
}

Biome* Biome::setLeafColor(int color)
{
	m_LeafColor = color;
	return this;
}

Biome* Biome::setName(const std::string & name)
{
	m_name = name;
	return this;
}

void Biome::initBiomes()
{
	rainForest = (new RainforestBiome)
		->setColor(0x08FA36)
		->setName("Rainforest")
		->setLeafColor(0x1FF458);

	swampland = (new SwampBiome)
		->setColor(0x07F9B2)
		->setName("Swampland")
		->setLeafColor(0x8BAF48);

	seasonalForest = (new Biome)
		->setColor(0x9BE023)
		->setName("Seasonal Forest");

	forest = (new ForestBiome)
		->setColor(0x056621)
		->setName("Forest")
		->setLeafColor(0x4EBA31);

	savanna = (new FlatBiome)
		->setColor(0xD9E023)
		->setName("Savanna");

	shrubland = (new Biome)
		->setColor(0xA1AD20)
		->setName("Shrubland");

	taiga = (new TaigaBiome)
		->setColor(0x2EB153)
		->setName("Taiga")
		->setSnowCovered()
		->setLeafColor(0x7BB731);

	desert = (new FlatBiome)
		->setColor(0xFA9418)
		->setName("Desert");

	plains = (new FlatBiome)
		->setColor(0xFFD910)
		->setName("Plains");

	iceDesert = (new FlatBiome)
		->setColor(0xFFED93)
		->setName("Ice Desert")
		->setSnowCovered()
		->setLeafColor(0xC4D339);

	tundra = (new Biome)
		->setColor(0x57EBF9)
		->setName("Tundra")
		->setSnowCovered()
		->setLeafColor(0xC4D339);

	recalc();
}

Feature* RainforestBiome::getTreeFeature(Random* pRandom)
{
	pRandom->genrand_int32(); //! unused result

	return new TreeFeature;
}

Feature* ForestBiome::getTreeFeature(Random* pRandom)
{
	if (pRandom->nextInt(5) == 0)
		return new BirchFeature;

	// @NOTE: Here would be code for a big tree with random->nextInt(3).
	// But PE doesn't have big trees

	return new TreeFeature;
}

Feature* TaigaBiome::getTreeFeature(Random* pRandom)
{
	if (pRandom->nextInt(3) == 0)
		return new PineFeature;
	
	return new SpruceFeature;
}
