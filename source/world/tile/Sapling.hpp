/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "Bush.hpp"

class Sapling : public Bush
{
public:
	Sapling(int id, int texture);

	int getTexture(int dir, int data) const override;
	void tick(Level*, int x, int y, int z, Random*) override;

	void growTree(Level*, int x, int y, int z, Random*);
	bool maybeGrowTree(Level*, int x, int y, int z, Random*);
};
