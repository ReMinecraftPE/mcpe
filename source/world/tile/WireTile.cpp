/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "WireTile.hpp"
#include "world/level/Level.hpp"

WireTile::WireTile(int id) : Tile(id, Material::decoration)
{
	m_TextureFrame = 0;
}
