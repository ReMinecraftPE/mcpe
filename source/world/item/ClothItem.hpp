/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/
#pragma once

#include "TileItem.hpp"

class ClothItem : public TileItem
{
public:
	ClothItem(int id);
	std::string getDescriptionId(ItemInstance* item) override;
	int getIcon(const ItemInstance*) const override;
	int getLevelDataForAuxValue(int x) override;
};
