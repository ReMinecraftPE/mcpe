/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/
#pragma once

#include "Item.hpp"

class TilePlanterItem : public Item
{
public:
	TilePlanterItem(int id, int place);

	bool useOn(ItemInstance*, Player*, Level*, const TilePos& pos, Facing::Name face) const override;

public:
	int m_tile;
};
