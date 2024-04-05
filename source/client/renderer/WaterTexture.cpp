/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "DynamicTexture.hpp"
#include "world/tile/Tile.hpp"

WaterTexture::WaterTexture() : DynamicTexture(Tile::water->m_TextureFrame)
{
	field_40C = 0;
	field_410 = 0;

	const int tsts = m_tileSize * m_tileSize;
	m_data1 = new float[tsts];
	m_data2 = new float[tsts];
	m_data3 = new float[tsts];
	m_data4 = new float[tsts];

	for (int i = 0; i < tsts; i++)
	{
		m_data1[i] = 0.0f;
		m_data2[i] = 0.0f;
		m_data3[i] = 0.0f;
		m_data4[i] = 0.0f;
	}
}

WaterTexture::~WaterTexture()
{
	SAFE_DELETE(m_data1);
	SAFE_DELETE(m_data2);
	SAFE_DELETE(m_data3);
	SAFE_DELETE(m_data4);
}

void WaterTexture::tick()
{
	const int ts = m_tileSize;
	const int tsm = ts - 1;
	const int tsts = ts * ts;
	// NOTE: ts MUST be a power of 2!

	for (int x = 0; x < ts; x++)
	{
		for (int y = 0; y < ts; y++)
		{
			float f = 0.0f;
			for (int i = x - 1; i <= x + 1; i++)
				f += m_data1[ts * (y & tsm) + (i & tsm)];

			m_data2[ts * y + x] = f / 3.3f + m_data3[ts * y + x] * 0.8f;
		}
	}

	for (int x = 0; x < ts; x++)
	{
		for (int y = 0; y < ts; y++)
		{
			m_data3[ts * y + x] += m_data4[ts * y + x] * 0.05f;

			if (m_data3[ts * y + x] < 0)
				m_data3[ts * y + x] = 0;

			m_data4[ts * y + x] -= 0.1f;
			if (Mth::random() < 0.05f)
				m_data4[ts * y + x] = 0.5f;
		}
	}

	std::swap(m_data1, m_data2);

	for (int i = 0; i < tsts; i++)
	{
		float m = m_data1[i];
		if (m < 0.0f)
			m = 0.0f;
		if (m > 1.0f)
			m = 1.0f;

		m = m * m;

		int r, g, b, a;

		r = int(32.0f  + 32.0f * m);
		g = int(64.0f  + 50.0f * m);
		a = int(146.0f + 50.0f * m);
		b = 255;
		
		m_pixels[i * 4 + 0] = uint8_t(r);
		m_pixels[i * 4 + 1] = uint8_t(g);
		m_pixels[i * 4 + 2] = uint8_t(b);
		m_pixels[i * 4 + 3] = uint8_t(a);
	}
}
