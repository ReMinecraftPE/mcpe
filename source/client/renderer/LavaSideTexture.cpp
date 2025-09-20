/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "DynamicTexture.hpp"
#include "world/tile/Tile.hpp"

LavaSideTexture::LavaSideTexture() : DynamicTexture(Tile::lava->m_TextureFrame + 1)
{
	field_14 = 0;
	field_18 = 0;
	field_1C = 0;

	m_data1 = new float[256];
	m_data2 = new float[256];
	m_data3 = new float[256];
	m_data4 = new float[256];
	m_textureSize = 2;

	for (int i = 0; i < 256; i++)
	{
		m_data1[i] = 0.0f;
		m_data2[i] = 0.0f;
		m_data3[i] = 0.0f;
		m_data4[i] = 0.0f;
	}
}

LavaSideTexture::~LavaSideTexture()
{
	SAFE_DELETE(m_data1);
	SAFE_DELETE(m_data2);
	SAFE_DELETE(m_data3);
	SAFE_DELETE(m_data4);
}

void LavaSideTexture::tick()
{
	field_1C++;

	for (int x = 0; x < 16; x++)
	{
		for (int y = 0; y < 16; y++)
		{
			float f = 0.0f;
			int ax = int(Mth::sin((float(x) * float(M_PI) * 2) / 16.0f) * 1.2f);
			int ay = int(Mth::sin((float(y) * float(M_PI) * 2) / 16.0f) * 1.2f);

			for (int bx = x - 1; bx <= x + 1; bx++)
			{
				for (int by = y - 1; by <= y + 1; by++)
				{
					int k2 = bx + ay & 0xf;
					int i3 = by + ax & 0xf;
					f += m_data1[k2 + i3 * 16];
				}
			}

			m_data2[x + y * 16] = f / 10.0f + ((m_data3[(x & 0xf) + (y + 0 & 0xf) * 16] + m_data3[(x + 1 & 0xf) + (y & 0xf) * 16] + m_data3[(x + 1 & 0xf) + (y + 1 & 0xf) * 16] + m_data3[(x & 0xf) + (y + 1 & 0xf) * 16]) * 0.25f) * 0.8f;
			m_data3[x + y * 16] += m_data4[x + y * 16] * 0.01f;

			if (m_data3[x + y * 16] < 0.0f)
				m_data3[x + y * 16] = 0.0f;

			m_data4[x + y * 16] -= 0.06f;
			if (Mth::random() < 0.005f)
				m_data4[x + y * 16] = 1.5f;
		}
	}

	std::swap(m_data1, m_data2);

	for (int i = 0; i < 256; i++)
	{
		float x1 = m_data1[(i - 16 * (field_1C / 3)) & 0xFF] * 2.0f;
		if (x1 > 1.0f)
			x1 = 1.0f;
		if (x1 < 0.0f)
			x1 = 0.0f;

		m_pixels[i * 4 + 0] = int(155.0f + 100.0f * x1);
		m_pixels[i * 4 + 1] = int(255.0f * x1 * x1);
		m_pixels[i * 4 + 2] = int(128.0f * x1 * x1 * x1 * x1);
		m_pixels[i * 4 + 3] = 255;
	}
}
