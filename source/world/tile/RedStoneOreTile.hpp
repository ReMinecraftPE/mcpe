/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "Tile.hpp"

class RedStoneOreTile : public Tile
{
public:
	RedStoneOreTile(int id, int texture, bool bLit);

	int getResource(int, Random*) override;
	int getResourceCount(Random*) override;
	int getSpawnResourcesAuxValue(int) override;
	int getTickDelay() override;
	void animateTick(Level*, int x, int y, int z, Random*) override;
	void tick(Level*, int x, int y, int z, Random*) override;
	void attack(Level*, int x, int y, int z, Player*) override;
	int use(Level*, int x, int y, int z, Player*) override;
	void stepOn(Level*, int x, int y, int z, Entity*) override;

	int poofParticles(Level*, int x, int y, int z);
	void interact(Level*, int x, int y, int z);

	bool m_bLit;
};
