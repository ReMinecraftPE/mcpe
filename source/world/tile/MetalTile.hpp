/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "Tile.hpp"

class MetalTile : public Tile
{
public:
	MetalTile(int ID, int texture, Material*);

	virtual int getTexture(int dir) const override;
};
