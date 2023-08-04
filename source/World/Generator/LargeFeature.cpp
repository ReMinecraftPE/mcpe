#include "LargeFeature.hpp"

LargeFeature::LargeFeature()
{
}

LargeFeature::~LargeFeature()
{
}

void LargeFeature::apply(ChunkSource* csrc, Level* level, int x, int z, uint8_t* tiles, int unk)
{
	m_random.setSeed(level->getSeed());

	int r1 = 2 * (m_random.nextLong() / 2) + 1;
	int r2 = 2 * (m_random.nextLong() / 2) + 1;

	for (int ax = x - m_radius; ax <= x + m_radius; ax++)
	{
		for (int az = z - m_radius; az <= z + m_radius; az++)
		{
			m_random.setSeed((r1 * ax + r2 * az) & level->getSeed());
			addFeature(level, ax, az, x, z, tiles, unk);
		}
	}
}
