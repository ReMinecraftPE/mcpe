/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "DynamicTexture.hpp"
#include "world/tile/Tile.hpp"

// Decompiled from MCPE v0.6.1.

LavaTexture::LavaTexture() : DynamicTexture(Tile::lava->m_TextureFrame)
{
	field_14 = 0;
	field_18 = 0;

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

LavaTexture::~LavaTexture()
{
	SAFE_DELETE(m_data1);
	SAFE_DELETE(m_data2);
	SAFE_DELETE(m_data3);
	SAFE_DELETE(m_data4);
}

void LavaTexture::tick()
{
	const int ts = m_tileSize;
	const int tsm = ts - 1;
	const int tsts = ts * ts;
	// NOTE: ts MUST be a power of 2!

	for (int x = 0; x < ts; x++)
	{
		for (int y = 0; y < ts; y++)
		{
			float f = 0.0F;
			int ax = int(Mth::sin((float(x) * float(M_PI) * 2) / 16.0f) * 1.2f);
			int ay = int(Mth::sin((float(y) * float(M_PI) * 2) / 16.0f) * 1.2f);

			for (int bx = x - 1; bx <= x + 1; bx++)
			{
				for (int by = y - 1; by <= y + 1; by++)
				{
					int k2 = bx + ay & tsm;
					int i3 = by + ax & tsm;
					f += m_data1[k2 + i3 * ts];
				}
			}

			m_data2[x + y * ts] = f / 10.0f + ((m_data3[(x & tsm) + (y + 0 & tsm) * ts] + m_data3[(x + 1 & tsm) + (y & tsm) * ts] + m_data3[(x + 1 & tsm) + (y + 1 & tsm) * ts] + m_data3[(x & tsm) + (y + 1 & tsm) * ts]) * 0.25f) * 0.8f;
			m_data3[x + y * ts] += m_data4[x + y * ts] * 0.01f;

			if (m_data3[x + y * ts] < 0.0f)
				m_data3[x + y * ts] = 0.0f;

			m_data4[x + y * ts] -= 0.06f;
			if (Mth::random() < 0.005f)
				m_data4[x + y * ts] = 1.5f;
		}
	}

	std::swap(m_data1, m_data2);

	for (int i = 0; i < tsts; i++)
	{
		float x1 = m_data1[i] * 2.0f;
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
