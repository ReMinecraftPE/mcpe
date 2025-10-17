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
	RedStoneOreTile(TileID id, int texture, bool bLit);

	int getResource(TileData, Random*) const override;
	int getResourceCount(Random*) const override;
	int getSpawnResourcesAuxValue(int) const override;
	int getTickDelay() const override;
	void animateTick(Level*, const TilePos& pos, Random*) override;
	void tick(Level*, const TilePos& pos, Random*) override;
	void attack(Level*, const TilePos& pos, Player*) override;
	bool use(Level*, const TilePos& pos, Player*) override;
	void stepOn(Level*, const TilePos& pos, Entity*) override;

	int poofParticles(Level*, const TilePos& pos);
	void interact(Level*, const TilePos& pos);

	bool m_bLit;
};
