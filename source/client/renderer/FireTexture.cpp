/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include <cstring>
#include "DynamicTexture.hpp"
#include "world/tile/Tile.hpp"

// Ported from v0.6.1

FireTexture::FireTexture(int a2) : DynamicTexture(Tile::fire->m_TextureFrame + 16 * a2)
{
	m_data1 = new float[320];
	m_data2 = new float[320];
	memset(m_data1, 0, sizeof(float) * 320);
	memset(m_data2, 0, sizeof(float) * 320);
}

FireTexture::~FireTexture()
{
	SAFE_DELETE_ARRAY(m_data1);
	SAFE_DELETE_ARRAY(m_data2);
}

void FireTexture::tick()
{
    for (int i = 0; i < 16; i++)
    {
        for (int j = 0; j < 20; j++)
        {
            int l = 18;
            float f1 = m_data1[i + ((j + 1) % 20) * 16] * l;
            for (int i1 = i - 1; i1 <= i + 1; i1++)
            {
                for (int k1 = j; k1 <= j + 1; k1++)
                {
                    int i2 = i1;
                    int k2 = k1;
                    if (i2 >= 0 && k2 >= 0 && i2 < 16 && k2 < 20)
                    {
                        f1 += m_data1[i2 + k2 * 16];
                    }
                    l++;
                }
            }

            m_data2[i + j * 16] = f1 / 25.2f;
            if (j >= 19)
            {
                // the original disassembly used things like HIBYTE(), LOBYTE() etc.
                union
                {
                    uint32_t x;
                    uint8_t b[4];
                }
                a;

                a.x = m_random.genrand_int32();
                this->m_data2[i + j * 16] = 0.2f + (((a.b[3] / 256.0f) * 0.1f) + ((((a.b[0] / 256.0f) * (a.b[1] / 256.0f)) * (a.b[2] / 256.0f)) * 4.0f));
            }
        }
    }

    std::swap(m_data1, m_data2);

    for (int i = 0; i < 256; i++)
    {
        float x = this->m_data1[i] * 1.8f;
        if (x > 1.0f)
            x = 1.0f;
        if (x < 0.0f)
            x = 0.0f;

        m_pixels[4 * i + 0] = int(x * 155.0f + 100.0f);
        m_pixels[4 * i + 1] = int(x * x * 255.0f);
        m_pixels[4 * i + 2] = int(x * x * x * x * x * x * x * x * x * x * 255.0f);
        m_pixels[4 * i + 3] = x >= 0.5f ? 255 : 0;
    }
}
