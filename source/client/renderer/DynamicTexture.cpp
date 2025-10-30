/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include <cstring>

#include "DynamicTexture.hpp"

DynamicTexture::DynamicTexture(int a2) : m_textureIndex(a2)
{
	m_textureSize = 1;

	memset(m_pixels, 0, sizeof m_pixels);
}

bool DynamicTexture::bindTexture(Textures* pTextures)
{
	return pTextures->loadAndBindTexture(C_TERRAIN_NAME) != -1;
}

DynamicTexture::~DynamicTexture()
{

}
