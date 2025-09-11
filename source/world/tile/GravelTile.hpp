/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "SandTile.hpp"

class GravelTile : public SandTile
{
public:
	GravelTile(int ID, int texture, Material*);

	int getResource(int data, Random* random) const override;
};
