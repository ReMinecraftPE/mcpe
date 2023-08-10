/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "Tile.hpp"

class TransparentTile : public Tile
{
public:
	TransparentTile(int ID, int texture, Material*, bool bTransparent);

	virtual bool isSolidRender() override;
	virtual bool shouldRenderFace(LevelSource*, int, int, int, int) override;

public:
	bool m_bTransparent;
};
