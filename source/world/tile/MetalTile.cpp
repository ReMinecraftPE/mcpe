/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "MetalTile.hpp"
#include "world/level/Level.hpp"
#include "client/renderer/PatchManager.hpp"

MetalTile::MetalTile(int ID, int texture, Material* pMtl) : Tile(ID, pMtl)
{
	m_TextureFrame = texture;
}

// @NOTE: I think the MCPE devs were left dumbfounded by this. "Why did notch
// overload this function?" Well, fun fact, there used to be top, side and bottom
// textures for these tiles. :)
int MetalTile::getTexture(Facing::Name face) const
{
	int yoff = GetPatchManager()->GetMetalSideYOffset();
	if (yoff < 0 || m_ID == Tile::lapisBlock->m_ID)
		return m_TextureFrame;

	if (face == Facing::UP) return m_TextureFrame;
	if (face == Facing::DOWN) return m_TextureFrame + 16 * (yoff + 1);

	return m_TextureFrame + 16 * (yoff + 0);
}
