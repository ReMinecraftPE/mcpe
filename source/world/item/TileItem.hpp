/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/
#pragma once

#include "Item.hpp"

class TileItem : public Item
{
public:
	TileItem(int id);

	virtual std::string getDescriptionId();
	virtual std::string getDescriptionId(ItemInstance*);
	virtual bool useOn(ItemInstance*, Player*, Level*, const TilePos& pos, Facing::Name face);

public:
	int m_tile;
};
