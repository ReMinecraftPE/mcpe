/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "Feature.hpp"
#include "world/level/Level.hpp"

ClayFeature::ClayFeature(TileID id, int count)
{
    m_ID = id;
    m_count = count;
}

bool ClayFeature::place(Level* level, Random* random, const TilePos& pos)
{
    if (level->getMaterial(pos) != Material::water)
    {
        return false;
    }

    // @NOTE: This appears to be pretty much the same as the ore feature.

    float fAng = random->nextFloat() * float(M_PI);

    float d0 = float(pos.x + 8) + 0.125f * float(m_count) * Mth::sin(fAng);
    float d1 = float(pos.x + 8) - 0.125f * float(m_count) * Mth::sin(fAng);
    float d2 = float(pos.z + 8) - 0.125f * float(m_count) * Mth::cos(fAng);
    float d3 = float(pos.z + 8) - 0.125f * float(m_count) * Mth::cos(fAng);

    float d4 = float(pos.y + random->nextInt(3) + 2);
    float d5 = float(pos.y + random->nextInt(3) + 2);

    for (int i = 0; i <= m_count; i++)
    {
        float d6 = d0 + ((d1 - d0) * float(i)) / float(m_count);
        float d7 = d4 + ((d5 - d4) * float(i)) / float(m_count);
        float d8 = d2 + ((d3 - d2) * float(i)) / float(m_count);
        float d9 = (random->nextFloat() * float(m_count)) / 16.0f;

        // @NOTE: seems to be calculated twice??
        float radius_1 = float(Mth::sin((float(i) * float(M_PI)) / float(m_count)) + 1.0f) * d9 + 1.0f;
        float radius_2 = float(Mth::sin((float(i) * float(M_PI)) / float(m_count)) + 1.0f) * d9 + 1.0f;

        TilePos min(d6 - radius_1 / 2.0f,
                    d7 - radius_2 / 2.0f,
                    d6 - radius_1 / 2.0f);
        TilePos max(d6 + radius_1 / 2.0f,
                    d7 + radius_2 / 2.0f,
                    d6 + radius_1 / 2.0f);

        TilePos tp(min.x, max.y, max.z);

        for (tp.x = min.x; tp.x <= max.x; tp.x++)
        {
            for (tp.y = min.y; tp.y <= max.y; tp.y++)
            {
                for (tp.z = min.z; tp.z <= max.z; tp.z++)
                {
                    float d12 = ((float(tp.x) + 0.5f) - d6) / (radius_1 / 2.0f);
                    float d13 = ((float(tp.y) + 0.5f) - d7) / (radius_2 / 2.0f);
                    float d14 = ((float(tp.z) + 0.5f) - d8) / (radius_1 / 2.0f);
                    if (d12 * d12 + d13 * d13 + d14 * d14 >= 1.0f)
                        continue;

                    if (level->getTile(tp) == Tile::sand->m_ID)
                        level->setTileNoUpdate(tp, m_ID);
                }
            }
        }
    }

    return true;
}
