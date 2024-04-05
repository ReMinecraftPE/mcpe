/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "DynamicTexture.hpp"
#include "common/Utils.hpp"

DynamicTexture::DynamicTexture(int texIndex, int texSize)
{
	m_textureIndex = texIndex;
	m_textureSize = texSize;
	m_tileSize = Textures::terrainTileSize;
	m_pixelsSize = m_tileSize * m_tileSize * sizeof(uint32_t);
	m_pixels = new uint8_t[m_pixelsSize];
	memset(m_pixels, 0, m_pixelsSize);
}

void DynamicTexture::bindTexture(Textures* pTextures)
{
	pTextures->loadAndBindTexture(C_TERRAIN_NAME);
}

DynamicTexture::~DynamicTexture()
{
	delete[] m_pixels;
}
