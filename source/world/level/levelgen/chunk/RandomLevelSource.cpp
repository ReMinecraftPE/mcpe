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
#include "world/level/levelgen/chunk/PostprocessingManager.hpp"
#include "world/level/levelgen/chunk/ChunkViewSource.hpp"
#include "world/level/TileSource.hpp"

const float RandomLevelSource::SNOW_CUTOFF = 0.5f;
const float RandomLevelSource::SNOW_SCALE  = 0.3f;

float g_timeSpentInPostProcessing = 0;

struct RandomLevelSource::ThreadData
{
private:
	Random m_random;
	BiomeSource m_biomeSource;

public:
	ThreadData(Level* level)
		: m_random(),
		  m_biomeSource(level)
	{
	}

public:
	Random& getRandom()
	{
		return m_random;
	}

	BiomeSource& getBiomeSource()
	{
		return m_biomeSource;
	}

public:
	static ThreadData* Create(Level* level)
	{
		return new ThreadData(level);
	}
};

RandomLevelSource::RandomLevelSource(Level* level, Dimension* dimension, uint32_t seed) :
	ChunkSource(level, dimension, 16),
	m_threadData(std::bind(&RandomLevelSource::ThreadData::Create, level)),
	m_random(seed),
	m_perlinNoise1(&m_random, 16),
	m_perlinNoise2(&m_random, 16),
	m_perlinNoise3(&m_random, 8),
	m_perlinNoise4(&m_random, 4),
	m_perlinNoise5(&m_random, 4),
	m_perlinNoise6(&m_random, 10),
	m_perlinNoise7(&m_random, 16),
	m_perlinNoise8(&m_random, 8)
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
}

LevelChunk* RandomLevelSource::requestChunk(const ChunkPos& pos, LoadMode loadMode)
{
	if (loadMode == Deferred)
		return new LevelChunk(*m_level, *m_dimension, pos, false);

	return nullptr;
}

void RandomLevelSource::loadChunk(LevelChunk& chunk)
{
	ThreadData& threadData = m_threadData.getLocal();

	threadData.getRandom().init_genrand(341872712 * chunk.getPos().x + 132899541 * chunk.getPos().z);

	Biome** pBiomeBlock = threadData.getBiomeSource().getBiomeBlock(TilePos(chunk.getPos(), 0), 16, 16);
	prepareHeights(&chunk, threadData.getBiomeSource().field_4);
	buildSurfaces(&chunk);

	// @NOTE: Java Edition Beta 1.6 uses the m_largeCaveFeature.
#ifdef FEATURE_CAVES
	m_largeCaveFeature.apply(this, m_pLevel, tilePos.x, tilePos.z, pLevelData, 0);
#endif

	chunk.recalcHeightmap();
	chunk.setGenerator(*this);
	chunk.setSaved();
	chunk.changeState(CS_GENERATING, CS_GENERATED);
}

void RandomLevelSource::postProcessMobsAt(TileSource* tileSource, int, int, Random& random)
{
	// TODO
}

static const int HEIGHTS_SIZE_X = 5;
static const int HEIGHTS_SIZE_Y = 17;
static const int HEIGHTS_SIZE_Z = 5;

void RandomLevelSource::getHeights(float* fptr, int a3, int a4, int a5)
{
	ThreadData& threadData = m_threadData.getLocal();

	float* bsf4 = threadData.getBiomeSource().field_4;
	float* bsf8 = threadData.getBiomeSource().field_8;

	constexpr float C_MAGIC_1 = 684.412f;

	// TODO: calculate the size beforehand instead of doing this
	float* perlinData1 = nullptr;
	float* perlinData2 = nullptr;
	float* perlinData3 = nullptr;
	float* perlinData4 = nullptr;
	float* perlinData5 = nullptr;

	perlinData1 = m_perlinNoise6.getRegion(perlinData1, a3, a5, float(HEIGHTS_SIZE_X), float(HEIGHTS_SIZE_Z), 1.121f, 1.121f, 0.5f);
	perlinData2 = m_perlinNoise7.getRegion(perlinData2, a3, a5, float(HEIGHTS_SIZE_X), float(HEIGHTS_SIZE_Z), 200.0f, 200.0f, 0.5f);
	perlinData3 = m_perlinNoise3.getRegion(perlinData3, float(a3), float(a4), float(a5), float(HEIGHTS_SIZE_X), float(HEIGHTS_SIZE_Y), float(HEIGHTS_SIZE_Z), 8.5552f, 4.2776f, 8.5552f);
	perlinData4 = m_perlinNoise1.getRegion(perlinData4, float(a3), float(a4), float(a5), float(HEIGHTS_SIZE_X), float(HEIGHTS_SIZE_Y), float(HEIGHTS_SIZE_Z), C_MAGIC_1, C_MAGIC_1, C_MAGIC_1);
	perlinData5 = m_perlinNoise2.getRegion(perlinData5, float(a3), float(a4), float(a5), float(HEIGHTS_SIZE_X), float(HEIGHTS_SIZE_Y), float(HEIGHTS_SIZE_Z), C_MAGIC_1, C_MAGIC_1, C_MAGIC_1);

	int k1 = 0;
	int l1 = 0;
	int i2 = 16 / HEIGHTS_SIZE_X;
	for (int j2 = 0; j2 < HEIGHTS_SIZE_X; j2++)
	{
		int k2 = j2 * i2 + i2 / 2;
		for (int l2 = 0; l2 < HEIGHTS_SIZE_Z; l2++)
		{
			int i3 = l2 * i2 + i2 / 2;
			float d2 = bsf4[k2 * 16 + i3];
			float d3 = bsf8[k2 * 16 + i3] * d2;
			float d4 = 1.0f - d3;
			d4 *= d4;
			d4 *= d4;
			d4 = 1.0f - d4;
			float d5 = (perlinData1[l1] + 256.0f) / 512.0f;
			d5 *= d4;
			if (d5 > 1.0f)
				d5 = 1.0f;
			float d6 = perlinData2[l1] / 8000.0f;
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
			d6 = (d6 * (float)HEIGHTS_SIZE_Y) / 16.0f;
			float d7 = (float)HEIGHTS_SIZE_Y / 2.0f + d6 * 4.0f;
			l1++;
			for (int j3 = 0; j3 < HEIGHTS_SIZE_Y; j3++)
			{
				float d8 = 0.0f;
				float d9 = (((float)j3 - d7) * 12.0f) / d5;
				if (d9 < 0.0f)
				{
					d9 *= 4.0f;
				}
				float d10 = perlinData4[k1] / 512.0f;
				float d11 = perlinData5[k1] / 512.0f;
				float d12 = (perlinData3[k1] / 10.0f + 1.0f) / 2.0f;
				if (d12 < 0.0f)
					d8 = d10;
				else if (d12 > 1.0f)
					d8 = d11;
				else
					d8 = d10 + (d11 - d10) * d12;
				d8 -= d9;
				if (j3 > HEIGHTS_SIZE_Y - 4)
				{
					float d13 = (float)(j3 - (HEIGHTS_SIZE_Y - 4)) / 3.0f;
					d8 = d8 * (1.0f - d13) + -10.0f * d13;
				}
				fptr[k1] = d8;
				k1++;
			}

		}

	}

	delete perlinData1;
	delete perlinData2;
	delete perlinData3;
	delete perlinData4;
	delete perlinData5;
}

void RandomLevelSource::prepareHeights(LevelChunk* chunk, float* biomeData)
{
	float heightData[HEIGHTS_SIZE_X * HEIGHTS_SIZE_Y * HEIGHTS_SIZE_Z];
	getHeights(heightData, chunk->getPos().x * 4, 0, chunk->getPos().z * 4);

	TileID* tiles = chunk->getTiles();

	for (int i = 0; i <= HEIGHTS_SIZE_X; i++)
	{
		for (int j = 0; j <= HEIGHTS_SIZE_Z; j++)
		{
			for (int k = 0; k <= HEIGHTS_SIZE_Y; k++)
			{
				float v24 = heightData[85 * i + 17 * j + k];
				float v23 = heightData[85 * i + 17 + 17 * j + k];
				float v22 = heightData[85 * i + 85 + 17 * j + k];
				float v21 = heightData[85 * i + 102 + 17 * j + k];
				float v20 = (heightData[85 * i + 1 + 17 * j + k] - v24) * 0.125f;
				float v19 = (heightData[85 * i + 18 + 17 * j + k] - v23) * 0.125f;
				float v18 = (heightData[85 * i + 86 + 17 * j + k] - v22) * 0.125f;
				float v17 = (heightData[85 * i + 103 + 17 * j + k] - v21) * 0.125f;
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
								if (biomeData[64 * i + 16 * m + 4 * j + n] < 0.5f && 8 * k + l == 63)
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

void RandomLevelSource::buildSurfaces(LevelChunk* chunk)
{
	ThreadData& threadData = m_threadData.getLocal();

	TileID* tiles = chunk->getTiles();
	Biome** biomes = threadData.getBiomeSource().getBiomeBlock(TilePos(chunk->getPos(), 0), 16, 16);

	float perlinData1[256];
	float perlinData2[256];
	float perlinData3[256];

	m_perlinNoise4.getRegion(perlinData1, float(chunk->getPos().x) * 16.0f, float(chunk->getPos().z) * 16.0f, 0.0f, 16, 16, 1, 1.0f / 32.0f, 1.0f / 32.0f, 1.0f);
	m_perlinNoise4.getRegion(perlinData2, float(chunk->getPos().x) * 16.0f, 109.01f, float(chunk->getPos().z) * 16.0f, 16, 1, 16, 1.0f / 32.0f, 1.0f, 1.0f / 32.0f);
	m_perlinNoise5.getRegion(perlinData3, float(chunk->getPos().x) * 16.0f, float(chunk->getPos().z) * 16.0f, 0.0f, 16, 16, 1, 1.0f / 16.0f, 1.0f / 16.0f, 1.0f / 16.0f);

	// @NOTE: Again, extracted from Java Beta 1.6. Probably accurate
	constexpr int byte0 = 64;
	//constexpr float d = 0.03125f;
	for (int k = 0; k < 16; k++)
	{
		for (int l = 0; l < 16; l++)
		{
			Biome* pBiome = biomes[k + l * 16];
			bool flag = perlinData1[k + l * 16] + m_random.nextFloat() * 0.2f > 0.0f;
			bool flag1 = perlinData2[k + l * 16] + m_random.nextFloat() * 0.2f > 3.0f;
			int i1 = (int)(perlinData3[k + l * 16] / 3.0f + 3.0f + m_random.nextFloat() * 0.25f);
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

bool RandomLevelSource::postProcess(ChunkViewSource& chunkViewSource)
{
	LevelChunk* chunk = getExistingChunk(ChunkPos(chunkViewSource.getBounds().getMin().x + 1, chunkViewSource.getBounds().getMin().z + 1));
	assert(chunk);

	PostprocessingManager::ScopedLock lock = PostprocessingManager::getInstance().tryLock(chunk->getPos());
	if (!lock.isValid())
		return false;

	ThreadData& threadData = m_threadData.getLocal();
	Random& random = threadData.getRandom();

	TileSource source(chunkViewSource.getLevel(), chunkViewSource.getDimension(), chunkViewSource, false, true);

	TileTickingQueue tileTickingQueue;
	source.setTickingQueue(tileTickingQueue);

	SandTile::instaFall = true;

	TilePos tp = TilePos(chunk->getPos(), 0);

	//LOG_I("Post-Processing %d, %d", pos.x, pos.z);
	Biome* pBiome = threadData.getBiomeSource().getBiome(tp + 16);

	random.setSeed(m_level->getSeed());
	int32_t x1 = 1 + 2 * (random.nextInt() / 2);
	int32_t x2 = 1 + 2 * (random.nextInt() / 2);
	random.setSeed((int32_t(chunk->getPos().x) * x1 + int32_t(chunk->getPos().z) * x2) ^ m_level->getSeed());

	// @NOTE: I can't put the random calls _in_ the argument list - args are evaluated right to left I believe

	for (int i = 0; i < 10; i++)
	{
		int xo = random.nextInt(16);
		int yo = random.nextInt(128);
		int zo = random.nextInt(16);
		ClayFeature(Tile::clay->m_ID, 32).place(&source, &random, TilePos(tp.x + xo, yo, tp.z + zo));
	}

	// Start of ore generation

	for (int i = 0; i < 20; i++)
	{
		int xo = random.nextInt(16);
		int yo = random.nextInt(128);
		int zo = random.nextInt(16);
		OreFeature(Tile::dirt->m_ID, 32).place(&source, &random, TilePos(tp.x + xo, yo, tp.z + zo));
	}
	for (int i = 0; i < 10; i++)
	{
		int xo = random.nextInt(16);
		int yo = random.nextInt(128);
		int zo = random.nextInt(16);
		OreFeature(Tile::gravel->m_ID, 32).place(&source, &random, TilePos(tp.x + xo, yo, tp.z + zo));
	}
	for (int i = 0; i < 20; i++)
	{
		int xo = random.nextInt(16);
		int yo = random.nextInt(128);
		int zo = random.nextInt(16);
		OreFeature(Tile::coalOre->m_ID, 16).place(&source, &random, TilePos(tp.x + xo, yo, tp.z + zo));
	}
	for (int i = 0; i < 20; i++)
	{
		int xo = random.nextInt(16);
		int yo = random.nextInt(64);
		int zo = random.nextInt(16);
		OreFeature(Tile::ironOre->m_ID, 8).place(&source, &random, TilePos(tp.x + xo, yo, tp.z + zo));
	}
	for (int i = 0; i < 2; i++)
	{
		int xo = random.nextInt(16);
		int yo = random.nextInt(32);
		int zo = random.nextInt(16);
		OreFeature(Tile::goldOre->m_ID, 8).place(&source, &random, TilePos(tp.x + xo, yo, tp.z + zo));
	}
	for (int i = 0; i < 8; i++)
	{
		int xo = random.nextInt(16);
		int yo = random.nextInt(16);
		int zo = random.nextInt(16);
		OreFeature(Tile::redStoneOre->m_ID, 7).place(&source, &random, TilePos(tp.x + xo, yo, tp.z + zo));
	}
	for (int i = 0; i < 1; i++)
	{
		int xo = random.nextInt(16);
		int yo = random.nextInt(16);
		int zo = random.nextInt(16);
		OreFeature(Tile::emeraldOre->m_ID, 7).place(&source, &random, TilePos(tp.x + xo, yo, tp.z + zo));
	}
	for (int i = 0; i < 1; i++)
	{
		int xo = random.nextInt(16);
		int yo = random.nextInt(16) + random.nextInt(16);
		int zo = random.nextInt(16);
		OreFeature(Tile::lapisOre->m_ID, 6).place(&source, &random, TilePos(tp.x + xo, yo, tp.z + zo));
	}

	// End of ore generation
	// Start of tree generation

	float t1 = m_perlinNoise8.getValue(float(tp.x) / 2.0f, float(tp.z) / 2.0f);
	float t2 = random.nextFloat();
	int t3 = int((4.0f + t2 * 4.0f + t1 * 0.125f) / 3.0f);

	int treeCount = 0;
	if (random.nextInt(10) == 0)
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
		rng.x = random.nextInt(16) + tp.x + 8;
		rng.z = random.nextInt(16) + tp.z + 8;
		rng.y = source.getHeightmap(rng);

		Feature* pTreeFeature = pBiome->getTreeFeature(&random);
		if (pTreeFeature)
		{
			pTreeFeature->init(1.0f, 1.0f, 1.0f);
			pTreeFeature->place(&source, &random, rng);
			delete pTreeFeature;
		}
	}

	for (int i = 0; i < 2; i++)
	{
		int xo = random.nextInt(16);
		int yo = random.nextInt(128);
		int zo = random.nextInt(16);
		FlowerFeature(Tile::flower->m_ID).place(&source, &random, TilePos(tp.x + 8 + xo, yo, tp.z + 8 + zo));
	}

	if (random.nextInt(2) == 0)
	{
		int xo = random.nextInt(16);
		int yo = random.nextInt(128);
		int zo = random.nextInt(16);
		FlowerFeature(Tile::rose->m_ID).place(&source, &random, TilePos(tp.x + 8 + xo, yo, tp.z + 8 + zo));
	}

	if (random.nextInt(4) == 0)
	{
		int xo = random.nextInt(16);
		int yo = random.nextInt(128);
		int zo = random.nextInt(16);
		FlowerFeature(Tile::mushroom1->m_ID).place(&source, &random, TilePos(tp.x + 8 + xo, yo, tp.z + 8 + zo));
	}

	if (random.nextInt(8) == 0)
	{
		int xo = random.nextInt(16);
		int yo = random.nextInt(128);
		int zo = random.nextInt(16);
		FlowerFeature(Tile::mushroom2->m_ID).place(&source, &random, TilePos(tp.x + 8 + xo, yo, tp.z + 8 + zo));
	}

	for (int i = 0; i < 10; i++)
	{
		int xo = random.nextInt(16);
		int yo = random.nextInt(128);
		int zo = random.nextInt(16);
		ReedsFeature().place(&source, &random, TilePos(tp.x + 8 + xo, yo, tp.z + 8 + zo));
	}

	for (int i = 0; i < 50; i++)
	{
		int xo = random.nextInt(16);
		int yo = random.nextInt(random.nextInt(120) + 8);
		int zo = random.nextInt(16);
		SpringFeature(Tile::water->m_ID).place(&source, &random, TilePos(tp.x + 8 + xo, yo, tp.z + 8 + zo));
	}

	for (int i = 0; i < 20; i++)
	{
		int xo = random.nextInt(16);
		int yo = random.nextInt(random.nextInt(random.nextInt(112) + 8) + 8);
		int zo = random.nextInt(16);
		SpringFeature(Tile::lava->m_ID).place(&source, &random, TilePos(tp.x + 8 + xo, yo, tp.z + 8 + zo));
	}

	int vegetationCount = 0;

	if (pBiome == Biome::desert)
		vegetationCount += 10;

	for (int i = 0; i < vegetationCount; i++)
	{
		int xo = random.nextInt(16);
		int yo = random.nextInt(128);
		int zo = random.nextInt(16);
		CactusFeature().place(&source, &random, TilePos(tp.x + 8 + xo, yo, tp.z + 8 + zo));
	}

	if (random.nextInt(32) == 0)
	{
		int xo = random.nextInt(16);
		int yo = random.nextInt(128);
		int zo = random.nextInt(16);
		PumpkinFeature().place(&source, &random, TilePos(tp.x + 8 + xo, yo, tp.z + 8 + zo));
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

		if (pBiome == Biome::rainForest && random.nextInt(3) != 0)
		{
			data = 2;
		}
		TilePos o(random.nextInt(16),
			random.nextInt(128),
			random.nextInt(16));
		VegetationFeature(Tile::tallGrass->m_ID, data).place(&source, &random, TilePos(tp.x + 8 + o.x, o.y, tp.z + 8 + o.z));
	}

	vegetationCount = 0;

	if (pBiome == Biome::desert)
		vegetationCount = 2;

	for (int i = 0; i < vegetationCount; i++)
	{
		int xo = random.nextInt(16);
		int yo = random.nextInt(128);
		int zo = random.nextInt(16);
		VegetationFeature(Tile::deadBush->m_ID, 0, 4).place(&source, &random, TilePos(tp.x + 8 + xo, yo, tp.z + 8 + zo));
	}
#endif
	float* tempBlock = threadData.getBiomeSource().getTemperatureBlock(tp.x + 8, tp.z + 8, 16, 16);
	for (int j19 = tp.x + 8; j19 < tp.x + 8 + 16; j19++)
	{
		for (int j22 = tp.z + 8; j22 < tp.z + 8 + 16; j22++)
		{
			int i24 = j19 - (tp.x + 8);
			int j25 = j22 - (tp.z + 8);

			int tsb = source.getTopSolidBlock(j19, j22, false);

			if (SNOW_CUTOFF > (tempBlock[i24 * 16 + j25] - SNOW_SCALE * (float(tsb - 64) / 64.0f)))
			{
				if (tsb >= 0 && tsb < C_MAX_Y && source.isEmptyTile(j19, tsb, j22))
				{
					Material* material = source.getMaterial(TilePos(j19, tsb - 1, j22));

					if (material->blocksMotion() &&
						material != Material::ice)
					{
						source.setTile(j19, tsb, j22, Tile::topSnow->m_ID);
					}
				}
			}
		}
	}

	tileTickingQueue.tickAllPendingTicks(source);

	SandTile::instaFall = false;

	chunk->setSaved();

	return true;
}
