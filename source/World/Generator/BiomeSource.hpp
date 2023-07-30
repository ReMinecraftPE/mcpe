#pragma once

#include "Biome.hpp"

class Level;
class BiomeSource
{
public:
	BiomeSource(Level*);
	BiomeSource();
	virtual ~BiomeSource();

	virtual Biome* getBiome(ChunkPos&);
	virtual Biome* getBiome(int, int);
	virtual Biome** getBiomeBlock(int, int, int, int);
	virtual Biome** getBiomeBlock(Biome**, int, int, int, int);
	virtual float* getTemperatureBlock(int, int, int, int);

public:
	float* field_4 = nullptr;
	float* field_8 = nullptr;
	float* field_C = nullptr;
	int field_10 = 0;
	int field_14 = 0;
	int field_18 = 0;
	int field_1C = 0;
	Biome** field_20 = nullptr;
	PerlinNoise* m_pPerlinNoise[3] = { nullptr };
	Random m_Random1;
	Random m_Random2;
	Random m_Random3;
};

