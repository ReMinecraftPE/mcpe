/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/
#pragma once

#include "Item.hpp"

class RedstoneItem : public Item
{
public:
	RedstoneItem(int id);

	virtual bool useOn(ItemInstance*, Player*, Level*, int, int, int, int);
};
