/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "Feature.hpp"
#include "world/level/Level.hpp"

ClayFeature::ClayFeature(int id, int count)
{
    m_ID = id;
    m_count = count;
}

bool ClayFeature::place(Level* level, Random* random, int x, int y, int z)
{
    if (level->getMaterial(x, y, z) != Material::water)
    {
        return false;
    }

    // @NOTE: This appears to be pretty much the same as the ore feature.

    float fAng = random->nextFloat() * float(M_PI);

    float d0 = float(x + 8) + 0.125f * float(m_count) * Mth::sin(fAng);
    float d1 = float(x + 8) - 0.125f * float(m_count) * Mth::sin(fAng);
    float d2 = float(z + 8) - 0.125f * float(m_count) * Mth::cos(fAng);
    float d3 = float(z + 8) - 0.125f * float(m_count) * Mth::cos(fAng);

    float d4 = float(y + random->nextInt(3) + 2);
    float d5 = float(y + random->nextInt(3) + 2);

    for (int i = 0; i <= m_count; i++)
    {
        float d6 = d0 + ((d1 - d0) * float(i)) / float(m_count);
        float d7 = d4 + ((d5 - d4) * float(i)) / float(m_count);
        float d8 = d2 + ((d3 - d2) * float(i)) / float(m_count);
        float d9 = (random->nextFloat() * float(m_count)) / 16.0f;

        // @NOTE: seems to be calculated twice??
        float radius_1 = float(Mth::sin((float(i) * float(M_PI)) / float(m_count)) + 1.0f) * d9 + 1.0f;
        float radius_2 = float(Mth::sin((float(i) * float(M_PI)) / float(m_count)) + 1.0f) * d9 + 1.0f;

        int minX = int(d6 - radius_1 / 2.0f);
        int maxX = int(d6 + radius_1 / 2.0f);
        int minY = int(d7 - radius_2 / 2.0f);
        int maxY = int(d7 + radius_2 / 2.0f);
        int minZ = int(d6 - radius_1 / 2.0f);
        int maxZ = int(d6 + radius_1 / 2.0f);

        for (int cx = minX; cx <= maxX; cx++)
        {
            for (int cy = minY; cy <= maxY; cy++)
            {
                for (int cz = minZ; cz <= maxZ; cz++)
                {
                    float d12 = ((float(cx) + 0.5f) - d6) / (radius_1 / 2.0f);
                    float d13 = ((float(cy) + 0.5f) - d7) / (radius_2 / 2.0f);
                    float d14 = ((float(cz) + 0.5f) - d8) / (radius_1 / 2.0f);
                    if (d12 * d12 + d13 * d13 + d14 * d14 >= 1.0f)
                        continue;

                    if (level->getTile(cx, cy, cz) == Tile::sand->m_ID)
                        level->setTileNoUpdate(cx, cy, cz, m_ID);
                }
            }
        }
    }

    return true;
}
