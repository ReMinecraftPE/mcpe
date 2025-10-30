/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "RandomLevelSource.hpp"

#include "common/Logger.hpp"
#include "world/level/Level.hpp"
#include "world/tile/SandTile.hpp"

const float RandomLevelSource::SNOW_CUTOFF = 0.5f;
const float RandomLevelSource::SNOW_SCALE  = 0.3f;

float g_timeSpentInPostProcessing = 0;

RandomLevelSource::RandomLevelSource(Level* level, int32_t seed, int version) :
	m_random(seed),
	m_perlinNoise1(&m_random, 16),
	m_perlinNoise2(&m_random, 16),
	m_perlinNoise3(&m_random, 8),
	m_perlinNoise4(&m_random, 4),
	m_perlinNoise5(&m_random, 4),
	m_perlinNoise6(&m_random, 10),
	m_perlinNoise7(&m_random, 16),
	m_perlinNoise8(&m_random, 8),
	m_pLevel(level)
{
	field_4 = false;
	field_19F0 = 1.0f;
	field_7280 = nullptr;
	field_7E84 = nullptr;
	field_7E88 = nullptr;
	field_7E8C = nullptr;
	field_7E90 = nullptr;
	field_7E94 = nullptr;

	LOG_I("Generating world with seed: %d", seed);

	for (int a = 0; a < 32; a++)
	{
		for (int b = 0; b < 32; b++)
		{
			field_9D8[a * 32 + b] = 0;
		}
	}

	field_7280 = new float[1024];

	Random random = m_random;
	LOG_I("random.get : %d", random.genrand_int32() >> 1);
}

// @BUG: Potential collisions.
inline int GetChunkHash(const ChunkPos& pos)
{
    int v6 = (pos.z & 0x7FFF) | ((pos.x & 0x7FFF) << 16) | (pos.x & 0x80000000), v7;
	if (pos.z >= 0)
		v7 = 0;
	else
		v7 = 0x8000;

	return v6 | v7;
}

LevelChunk* RandomLevelSource::getChunk(const ChunkPos& pos)
{
	int hashCode = GetChunkHash(pos);
	std::map<int, LevelChunk*>::iterator iter = m_chunks.find(hashCode);
	if (iter != m_chunks.end())
		return iter->second;

	// have to generate the chunk
	m_random.init_genrand(341872712 * pos.x + 132899541 * pos.z);

	TileID* pLevelData = new TileID[32768];

	LevelChunk* pChunk = new LevelChunk(m_pLevel, pLevelData, pos);
	m_chunks.insert(std::pair<int, LevelChunk*>(hashCode, pChunk));

	Biome** pBiomeBlock = m_pLevel->getBiomeSource()->getBiomeBlock(TilePos(pos, 0), 16, 16);
	prepareHeights(pos, pLevelData, nullptr, m_pLevel->getBiomeSource()->field_4);
	buildSurfaces(pos, pLevelData, pBiomeBlock);
	pChunk->recalcHeightmap();

	// @NOTE: Java Edition Beta 1.6 uses the m_largeCaveFeature.
#ifdef TEST_CAVES
	m_largeCaveFeature.apply(this, m_pLevel, pos.x, pos.z, pLevelData, 0);
#endif

	return pChunk;
}

LevelChunk* RandomLevelSource::getChunkDontCreate(const ChunkPos& pos)
{
	int hashCode = GetChunkHash(pos);
	std::map<int, LevelChunk*>::iterator iter = m_chunks.find(hashCode);
	if (iter != m_chunks.end())
		return iter->second;

	// have to create the chunk. Create an empty one

	TileID* pLevelData = new TileID[32768];
	memset (pLevelData, 0, sizeof *pLevelData * 32768);

	LevelChunk* pChunk = new LevelChunk(m_pLevel, pLevelData, pos);
	m_chunks.insert(std::pair<int, LevelChunk*>(hashCode, pChunk));

	return pChunk;
}

float* RandomLevelSource::getHeights(float* fptr, int a3, int a4, int a5, int a6, int a7, int a8)
{
	if (fptr == nullptr)
	{
		fptr = new float[a6 * a7 * a8];
	}

	float* bsf4 = m_pLevel->getBiomeSource()->field_4;
	float* bsf8 = m_pLevel->getBiomeSource()->field_8;

	constexpr float C_MAGIC_1 = 684.412f;

	field_7E90 = m_perlinNoise6.getRegion(field_7E90, a3, a5, a6, a8, 1.121f, 1.121f, 0.5f);
	field_7E94 = m_perlinNoise7.getRegion(field_7E94, a3, a5, a6, a8, 200.0f, 200.0f, 0.5f);
	field_7E84 = m_perlinNoise3.getRegion(field_7E84, float(a3), float(a4), float(a5), a6, a7, a8, 8.5552f,   4.2776f,   8.5552f);
	field_7E88 = m_perlinNoise1.getRegion(field_7E88, float(a3), float(a4), float(a5), a6, a7, a8, C_MAGIC_1, C_MAGIC_1, C_MAGIC_1);
	field_7E8C = m_perlinNoise2.getRegion(field_7E8C, float(a3), float(a4), float(a5), a6, a7, a8, C_MAGIC_1, C_MAGIC_1, C_MAGIC_1);

	int k1 = 0;
	int l1 = 0;
	int i2 = 16 / a6;
	for (int j2 = 0; j2 < a6; j2++)
	{
		int k2 = j2 * i2 + i2 / 2;
		for (int l2 = 0; l2 < a8; l2++)
		{
			int i3 = l2 * i2 + i2 / 2;
			float d2 = bsf4[k2 * 16 + i3];
			float d3 = bsf8[k2 * 16 + i3] * d2;
			float d4 = 1.0f - d3;
			d4 *= d4;
			d4 *= d4;
			d4 = 1.0f - d4;
			float d5 = (field_7E90[l1] + 256.0f) / 512.0f;
			d5 *= d4;
			if (d5 > 1.0f)
				d5 = 1.0f;
			float d6 = field_7E94[l1] / 8000.0f;
			if (d6 < 0.0f)
			{
				d6 = -d6 * 0.3f;
			}
			d6 = d6 * 3.0f - 2.0f;
			if (d6 < 0.0f)
			{
				d6 /= 2.0f;
				if (d6 < -1.0f)
					d6 = -1.0f;
				d6 /= 1.4f;
				d6 /= 2.0f;
				d5 = 0.0f;
			}
			else
			{
				if (d6 > 1.0f)
					d6 = 1.0f;
				d6 /= 8.0f;
			}
			if (d5 < 0.0f)
				d5 = 0.0f;
			d5 += 0.5f;
			d6 = (d6 * (float)a7) / 16.0f;
			float d7 = (float)a7 / 2.0f + d6 * 4.0f;
			l1++;
			for (int j3 = 0; j3 < a7; j3++)
			{
				float d8 = 0.0f;
				float d9 = (((float)j3 - d7) * 12.0f) / d5;
				if (d9 < 0.0f)
				{
					d9 *= 4.0f;
				}
				float d10 = field_7E88[k1] / 512.0f;
				float d11 = field_7E8C[k1] / 512.0f;
				float d12 = (field_7E84[k1] / 10.0f + 1.0f) / 2.0f;
				if (d12 < 0.0f)
					d8 = d10;
				else if (d12 > 1.0f)
					d8 = d11;
				else
					d8 = d10 + (d11 - d10) * d12;
				d8 -= d9;
				if (j3 > a7 - 4)
				{
					float d13 = (float)(j3 - (a7 - 4)) / 3.0f;
					d8 = d8 * (1.0f - d13) + -10.0f * d13;
				}
				fptr[k1] = d8;
				k1++;
			}

		}

	}

	return fptr;
}

void RandomLevelSource::prepareHeights(const ChunkPos& pos, TileID* tiles, void* huh, float* fptr)
{
	field_7280 = getHeights(field_7280, pos.x * 4, 0, pos.z * 4, 5, 17, 5);

	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			for (int k = 0; k < 16; k++)
			{
				float v24 = field_7280[85 * i + 17 * j + k];
				float v23 = field_7280[85 * i + 17 + 17 * j + k];
				float v22 = field_7280[85 * i + 85 + 17 * j + k];
				float v21 = field_7280[85 * i + 102 + 17 * j + k];
				float v20 = (field_7280[85 * i + 1 + 17 * j + k] - v24) * 0.125f;
				float v19 = (field_7280[85 * i + 18 + 17 * j + k] - v23) * 0.125f;
				float v18 = (field_7280[85 * i + 86 + 17 * j + k] - v22) * 0.125f;
				float v17 = (field_7280[85 * i + 103 + 17 * j + k] - v21) * 0.125f;
				for (int l = 0; l < 8; l++)
				{
					float v15 = v24;
					float v14 = v23;
					for (int m = 0; m < 4; m++)
					{
						int v12 = ((m + 4 * i) << 11) | (j << 9) | (l + 8 * k);
						float v11 = v15;
						for (int n = 0; n < 4; n++)
						{
							TileID tile = TILE_AIR;
							if (8 * k + l < 64)
							{
								if (fptr[64 * i + 16 * m + 4 * j + n] < 0.5f && 8 * k + l == 63)
									tile = Tile::ice->m_ID;
								else
									tile = Tile::calmWater->m_ID;
							}
							if (v11 > 0.0f)
								tile = Tile::rock->m_ID;
							
							tiles[v12] = tile;
							v12 += 128;
							v11 = v11 + (v14 - v15) * 0.25f;
						}
						v15 = v15 + (v22 - v24) * 0.25f;
						v14 = v14 + (v21 - v23) * 0.25f;
					}
					v24 = v24 + v20;
					v23 = v23 + v19;
					v22 = v22 + v18;
					v21 = v21 + v17;
				}
			}
		}
	}
}

void RandomLevelSource::buildSurfaces(const ChunkPos& pos, TileID* tiles, Biome** biomes)
{
	//return;
	m_perlinNoise4.getRegion(field_7284, float(pos.x) * 16.0f, float(pos.z) * 16.0f, 0.0f,    16, 16, 1, 1.0f / 32.0f, 1.0f / 32.0f, 1.0f);
	m_perlinNoise4.getRegion(field_7684, float(pos.x) * 16.0f, 109.01f, float(pos.z) * 16.0f, 16, 1, 16, 1.0f / 32.0f, 1.0f,         1.0f / 32.0f);
	m_perlinNoise5.getRegion(field_7A84, float(pos.x) * 16.0f, float(pos.z) * 16.0f, 0.0f,    16, 16, 1, 1.0f / 16.0f, 1.0f / 16.0f, 1.0f / 16.0f);

	// @NOTE: Again, extracted from Java Beta 1.6. Probably accurate
	constexpr int byte0 = 64;
	//constexpr float d = 0.03125f;
	for (int k = 0; k < 16; k++)
	{
		for (int l = 0; l < 16; l++)
		{
			Biome* pBiome = biomes[k + l * 16];
			bool flag = field_7284[k + l * 16] + m_random.nextFloat() * 0.2f > 0.0f;
			bool flag1 = field_7684[k + l * 16] + m_random.nextFloat() * 0.2f > 3.0f;
			int i1 = (int)(field_7A84[k + l * 16] / 3.0f + 3.0f + m_random.nextFloat() * 0.25f);
			int j1 = -1;

			TileID byte1 = pBiome->field_20;
			TileID byte2 = pBiome->field_21;

			for (int k1 = 127; k1 >= 0; k1--)
			{
				int l1 = (l * 16 + k) * 128 + k1;
				if (k1 <= int(0 + m_random.nextInt(5)))
				{
					tiles[l1] = Tile::unbreakable->m_ID;
					continue;
				}

				TileID byte3 = tiles[l1];
				if (byte3 == 0)
				{
					j1 = -1;
					continue;
				}

				if (byte3 != Tile::rock->m_ID)
					continue;

				if (j1 == -1)
				{
					if (i1 <= 0)
					{
						byte1 = 0;
						byte2 = Tile::rock->m_ID;
					}
					else if (k1 >= byte0 - 4 && k1 <= byte0 + 1)
					{
						byte1 = pBiome->field_20;
						byte2 = pBiome->field_21;
						if (flag1) {
							byte1 = 0;
							byte2 = Tile::gravel->m_ID;
						}
						if (flag) {
							byte1 = Tile::sand->m_ID;
							byte2 = Tile::sand->m_ID;
						}
					}

					if (k1 < byte0 && byte1 == 0)
						byte1 = Tile::calmWater->m_ID;

					j1 = i1;

					if (k1 >= byte0 - 1)
						tiles[l1] = byte1;
					else
						tiles[l1] = byte2;

					continue;
				}

				if (j1 <= 0)
					continue;

				j1--;
				tiles[l1] = byte2;

				if (j1 == 0 && byte2 == Tile::sand->m_ID)
				{
					j1 = m_random.nextInt(4);
					byte2 = Tile::sandStone->m_ID;
				}
			}
		}
	}
}

void RandomLevelSource::postProcess(ChunkSource* src, const ChunkPos& pos)
{
	m_pLevel->m_bPostProcessing = true;
	SandTile::instaFall = true;
	
	TilePos tp = TilePos(pos, 0);

	//LOG_I("Post-Processing %d, %d", pos.x, pos.z);
	Biome* pBiome = m_pLevel->getBiomeSource()->getBiome(tp + 16);
	int32_t seed = m_pLevel->getSeed();

	m_random.setSeed(seed);
	int32_t x1 = 1 + 2 * (m_random.nextInt() / 2);
	int32_t x2 = 1 + 2 * (m_random.nextInt() / 2);
	m_random.setSeed((int32_t(pos.x) * x1 + int32_t(pos.z) * x2) ^ seed);

	// @NOTE: I can't put the random calls _in_ the argument list - args are evaluated right to left I believe

	for (int i = 0; i < 10; i++)
	{
		TilePos o(m_random.nextInt(16),
		          m_random.nextInt(128),
		          m_random.nextInt(16));
		ClayFeature(Tile::clay->m_ID, 32).place(m_pLevel, &m_random, tp + o);
	}
	
	// Start of ore generation

	for (int i = 0; i < 20; i++)
	{
		TilePos o(m_random.nextInt(16),
		          m_random.nextInt(128),
		          m_random.nextInt(16));
		OreFeature(Tile::dirt->m_ID, 32).place(m_pLevel, &m_random, tp + o);
	}
	for (int i = 0; i < 10; i++)
	{
		TilePos o(m_random.nextInt(16),
		          m_random.nextInt(128),
		          m_random.nextInt(16));
		OreFeature(Tile::gravel->m_ID, 32).place(m_pLevel, &m_random, tp + o);
	}
	for (int i = 0; i < 20; i++)
	{
		TilePos o(m_random.nextInt(16),
		          m_random.nextInt(128),
		          m_random.nextInt(16));
		OreFeature(Tile::coalOre->m_ID, 16).place(m_pLevel, &m_random, tp + o);
	}
	for (int i = 0; i < 20; i++)
	{
		TilePos o(m_random.nextInt(16),
		          m_random.nextInt(64),
		          m_random.nextInt(16));
		OreFeature(Tile::ironOre->m_ID, 8).place(m_pLevel, &m_random, tp + o);
	}
	for (int i = 0; i < 2; i++)
	{
		TilePos o(m_random.nextInt(16),
		          m_random.nextInt(32),
		          m_random.nextInt(16));
		OreFeature(Tile::goldOre->m_ID, 8).place(m_pLevel, &m_random, tp + o);
	}
	for (int i = 0; i < 8; i++)
	{
		TilePos o(m_random.nextInt(16),
		          m_random.nextInt(16),
		          m_random.nextInt(16));
		OreFeature(Tile::redStoneOre->m_ID, 7).place(m_pLevel, &m_random, tp + o);
	}
	for (int i = 0; i < 1; i++)
	{
		TilePos o(m_random.nextInt(16),
		          m_random.nextInt(16),
		          m_random.nextInt(16));
		OreFeature(Tile::emeraldOre->m_ID, 7).place(m_pLevel, &m_random, tp + o);
	}
	for (int i = 0; i < 1; i++)
	{
		TilePos o(m_random.nextInt(16),
		          m_random.nextInt(16) + m_random.nextInt(16),
		          m_random.nextInt(16));
		OreFeature(Tile::lapisOre->m_ID, 6).place(m_pLevel, &m_random, tp + o);
	}

	// End of ore generation
	// Start of tree generation

	float t1 = m_perlinNoise8.getValue(float(tp.x) / 2.0f, float(tp.z) / 2.0f);
	float t2 = m_random.nextFloat();
	int t3 = int((4.0f + t2 * 4.0f + t1 * 0.125f) / 3.0f);

	int treeCount = 0;
	if (m_random.nextInt(10) == 0)
		treeCount++;

	if (pBiome == Biome::forest)
		treeCount += t3 + 2;

	if (pBiome == Biome::rainForest)
		treeCount += t3 + 2;

	if (pBiome == Biome::seasonalForest)
		treeCount += t3 + 1;

	if (pBiome == Biome::taiga)
		treeCount += t3 + 1;

	if (pBiome == Biome::desert)
		treeCount -= 20;

	if (pBiome == Biome::tundra)
		treeCount -= 20;

	if (pBiome == Biome::plains)
		treeCount -= 20;

	for (int i = 0; i < treeCount; i++)
	{
		TilePos rng;
		rng.x = m_random.nextInt(16) + tp.x + 8;
		rng.z = m_random.nextInt(16) + tp.z + 8;
		rng.y = m_pLevel->getHeightmap(rng);

		Feature* pTreeFeature = pBiome->getTreeFeature(&m_random);
		if (pTreeFeature)
		{
			pTreeFeature->init(1.0f, 1.0f, 1.0f);
			pTreeFeature->place(m_pLevel, &m_random, rng);
			delete pTreeFeature;
		}
	}

	for (int i = 0; i < 2; i++)
	{
		int xo = m_random.nextInt(16);
		int yo = m_random.nextInt(128);
		int zo = m_random.nextInt(16);
		FlowerFeature(Tile::flower->m_ID).place(m_pLevel, &m_random, TilePos(tp.x + 8 + xo, yo, tp.z + 8 + zo));
	}

	if (m_random.nextInt(2) == 0)
	{
		int xo = m_random.nextInt(16);
		int yo = m_random.nextInt(128);
		int zo = m_random.nextInt(16);
		FlowerFeature(Tile::rose->m_ID).place(m_pLevel, &m_random, TilePos(tp.x + 8 + xo, yo, tp.z + 8 + zo));
	}

	if (m_random.nextInt(4) == 0)
	{
		int xo = m_random.nextInt(16);
		int yo = m_random.nextInt(128);
		int zo = m_random.nextInt(16);
		FlowerFeature(Tile::mushroom1->m_ID).place(m_pLevel, &m_random, TilePos(tp.x + 8 + xo, yo, tp.z + 8 + zo));
	}

	if (m_random.nextInt(8) == 0)
	{
		int xo = m_random.nextInt(16);
		int yo = m_random.nextInt(128);
		int zo = m_random.nextInt(16);
		FlowerFeature(Tile::mushroom2->m_ID).place(m_pLevel, &m_random, TilePos(tp.x + 8 + xo, yo, tp.z + 8 + zo));
	}

	for (int i = 0; i < 10; i++)
	{
		int xo = m_random.nextInt(16);
		int yo = m_random.nextInt(128);
		int zo = m_random.nextInt(16);
		ReedsFeature().place(m_pLevel, &m_random, TilePos(tp.x + 8 + xo, yo, tp.z + 8 + zo));
	}

	for (int i = 0; i < 50; i++)
	{
		int xo = m_random.nextInt(16);
		int yo = m_random.nextInt(m_random.nextInt(120) + 8);
		int zo = m_random.nextInt(16);
		SpringFeature(Tile::water->m_ID).place(m_pLevel, &m_random, TilePos(tp.x + 8 + xo, yo, tp.z + 8 + zo));
	}

	for (int i = 0; i < 20; i++)
	{
		int xo = m_random.nextInt(16);
		int yo = m_random.nextInt(m_random.nextInt(m_random.nextInt(112) + 8) + 8);
		int zo = m_random.nextInt(16);
		SpringFeature(Tile::lava->m_ID).place(m_pLevel, &m_random, TilePos(tp.x + 8 + xo, yo, tp.z + 8 + zo));
	}

	int vegetationCount = 0;

	if (pBiome == Biome::desert)
		vegetationCount += 10;

	for (int i = 0; i < vegetationCount; i++)
	{
		int xo = m_random.nextInt(16);
		int yo = m_random.nextInt(128);
		int zo = m_random.nextInt(16);
		CactusFeature().place(m_pLevel, &m_random, TilePos(tp.x + 8 + xo, yo, tp.z + 8 + zo));
	}

	if (m_random.nextInt(32) == 0)
	{
		int xo = m_random.nextInt(16);
		int yo = m_random.nextInt(128);
		int zo = m_random.nextInt(16);
		PumpkinFeature().place(m_pLevel, &m_random, TilePos(tp.x + 8 + xo, yo, tp.z + 8 + zo));
	}
#ifdef FEATURE_PLANT_VEGGIES
	vegetationCount = 0;

	if (pBiome == Biome::forest)
		vegetationCount = 2;

	else if (pBiome == Biome::rainForest)
		vegetationCount = 10;

	else if (pBiome == Biome::seasonalForest)
		vegetationCount = 2;

	else if (pBiome == Biome::taiga)
		vegetationCount = 1;

	else if (pBiome == Biome::plains)
		vegetationCount = 10;

	for (int i = 0; i < vegetationCount; i++)
	{
		TileData data = 1;

		if (pBiome == Biome::rainForest && m_random.nextInt(3) != 0) 
		{
			data = 2;
		}
		TilePos o(m_random.nextInt(16),
		m_random.nextInt(128),
		m_random.nextInt(16));
		VegetationFeature(Tile::tallGrass->m_ID, data).place(m_pLevel, &m_random, TilePos(tp.x + 8 + o.x, o.y, tp.z + 8 + o.z));
	}

	vegetationCount = 0;

	if (pBiome == Biome::desert)
		vegetationCount = 2;

	for (int i = 0; i < vegetationCount; i++)
	{
		int xo = m_random.nextInt(16);
		int yo = m_random.nextInt(128);
		int zo = m_random.nextInt(16);
		VegetationFeature(Tile::deadBush->m_ID, 0, 4).place(m_pLevel, &m_random, TilePos(tp.x + 8 + xo, yo, tp.z + 8 + zo));
	}
#endif
	float* tempBlock = m_pLevel->getBiomeSource()->getTemperatureBlock(tp.x + 8, tp.z + 8, 16, 16);
	for (int j19 = tp.x + 8; j19 < tp.x + 8 + 16; j19++)
	{
		for (int j22 = tp.z + 8; j22 < tp.z + 8 + 16; j22++)
		{
			int i24 = j19 - (tp.x + 8);
			int j25 = j22 - (tp.z + 8);

			int tsb = m_pLevel->getTopSolidBlock(TilePos(j19, 0, j22));
			
			if (SNOW_CUTOFF > (tempBlock[i24 * 16 + j25] - SNOW_SCALE * (float(tsb - 64) / 64.0f)))
			{
				if (tsb >= 0 && tsb < C_MAX_Y && m_pLevel->isEmptyTile(TilePos(j19, tsb, j22)))
				{
					if (m_pLevel->getMaterial(TilePos(j19, tsb - 1, j22))->blocksMotion() &&
						m_pLevel->getMaterial(TilePos(j19, tsb - 1, j22)) != Material::ice)
					{
						m_pLevel->setTile(TilePos(j19, tsb, j22), Tile::topSnow->m_ID);
					}
				}
			}
		}
	}

	SandTile::instaFall = false;
	m_pLevel->m_bPostProcessing = false;
}

int RandomLevelSource::tick()
{
	return 0;
}

bool RandomLevelSource::shouldSave()
{
	return true;
}

bool RandomLevelSource::hasChunk(const ChunkPos& pos)
{
	return true;
}

LevelChunk* RandomLevelSource::create(const ChunkPos& pos)
{
	return getChunk(pos);
}

std::string RandomLevelSource::gatherStats()
{
	return "RandomLevelSource";
}
