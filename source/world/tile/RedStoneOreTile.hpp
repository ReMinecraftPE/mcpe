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

public:
	int getResource(TileData, Random*) const override;
	int getResourceCount(Random*) const override;
	int getSpawnResourcesAuxValue(int) const override;
	int getTickDelay() const override;
	void animateTick(TileSource*, const TilePos& pos, Random*) override;
	void tick(TileSource*, const TilePos& pos, Random*) override;
	void attack(TileSource*, const TilePos& pos, Player*) override;
	bool use(const TilePos& pos, Player*) override;
	void stepOn(TileSource*, const TilePos& pos, Entity*) override;

	int poofParticles(TileSource*, const TilePos& pos);
	void interact(TileSource*, const TilePos& pos);

public:
	bool m_bLit;
};
