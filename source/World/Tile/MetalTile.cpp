/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "Tile.hpp"
#include "Level.hpp"

MetalTile::MetalTile(int ID, int texture, Material* pMtl) : Tile(ID, pMtl)
{
	m_TextureFrame = texture;
}

// @NOTE: I think the MCPE devs were left dumbfounded by this. "Why did notch
// overload this function?" Well, fun fact, there used to be top, side and bottom
// textures for these tiles. :)
int MetalTile::getTexture(int dir)
{
	return m_TextureFrame;
}
