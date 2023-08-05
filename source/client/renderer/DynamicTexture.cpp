/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "DynamicTexture.hpp"
#include "client/common/Utils.hpp"

DynamicTexture::DynamicTexture(int a2) : m_textureIndex(a2)
{
	memset(m_pixels, 0, sizeof m_pixels);
}

void DynamicTexture::bindTexture(Textures* pTextures)
{
	pTextures->loadAndBindTexture(C_TERRAIN_NAME);
}

DynamicTexture::~DynamicTexture()
{

}
