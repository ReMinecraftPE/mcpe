/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "DynamicTexture.hpp"
#include "Tile.hpp"

WaterSideTexture::WaterSideTexture() : DynamicTexture(Tile::water->m_TextureFrame + 1)
{
	m_data1 = new float[0x400];
	m_data2 = new float[0x400];
	m_data3 = new float[0x400];
	m_data4 = new float[0x400];
	m_textureSize = 2;

	for (int i = 0; i < 256; i++)
	{
		m_data1[i] = 0.0f;
		m_data2[i] = 0.0f;
		m_data3[i] = 0.0f;
		m_data4[i] = 0.0f;
	}
}

WaterSideTexture::~WaterSideTexture()
{
	SAFE_DELETE(m_data1);
	SAFE_DELETE(m_data2);
	SAFE_DELETE(m_data3);
	SAFE_DELETE(m_data4);
}

void WaterSideTexture::tick()
{
	field_414++;
	
	for (int x = 0; x < 16; x++)
	{
		for (int y = 0; y < 16; y++)
		{
			float f = 0.0f;
			for (int i = y - 2; i <= y; i++)
				f += m_data1[16 * (i & 0xF) + (x & 0xF)];

			m_data2[16 * y + x] = f / 3.2f + m_data3[16 * y + x] * 0.8f;
		}
	}

	for (int x = 0; x < 16; x++)
	{
		for (int y = 0; y < 16; y++)
		{
			m_data3[16 * y + x] += m_data4[16 * y + x] * 0.05f;

			if (m_data3[16 * y + x] < 0)
				m_data3[16 * y + x] = 0;

			m_data4[16 * y + x] -= 0.3f;
			if (Mth::random() < 0.2f)
				m_data4[16 * y + x] = 0.5f;
		}
	}

	std::swap(m_data1, m_data2);

	for (int i = 0; i < 256; i++)
	{
		float m = m_data1[(i - 16 * field_414) & 0xFF];
		if (m < 0.0f)
			m = 0.0f;
		if (m > 1.0f)
			m = 1.0f;

		m = m * m;

		int r, g, b, a;

		r = int(32.0f + 32.0f * m);
		g = int(64.0f + 50.0f * m);
		a = int(146.0f + 50.0f * m);
		b = 255;

		m_pixels[i * 4 + 0] = uint8_t(r);
		m_pixels[i * 4 + 1] = uint8_t(g);
		m_pixels[i * 4 + 2] = uint8_t(b);
		m_pixels[i * 4 + 3] = uint8_t(a);
	}
}
