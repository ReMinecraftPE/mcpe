/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "Tile.hpp"

class HalfTransparentTile : public Tile
{
public:
	HalfTransparentTile(int ID, int texture, Material*);

	virtual bool isSolidRender() const override;
	virtual bool shouldRenderFace(const LevelSource*, int, int, int, int) const override;

public:
	bool field_6C;
};
