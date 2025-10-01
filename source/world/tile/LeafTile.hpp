/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "TransparentTile.hpp"

class LeafTile : public TransparentTile
{
public:
	LeafTile(TileID id);
	~LeafTile();

private:
	void _tickDecayOld(Level* level, const TilePos& pos); // circa 0.1.0
	void _tickDecay(Level* level, const TilePos& pos); /// circa b1.7.3

public:
	int getColor(const LevelSource*, const TilePos& pos) const override;
	int getTexture(Facing::Name face, TileData data) const override;
	bool isSolidRender() const override;
	void onRemove(Level*, const TilePos& pos) override;
	void stepOn(Level*, const TilePos& pos, Entity*) override;
	void tick(Level*, const TilePos& pos, Random*) override;
	int getResource(TileData data, Random* random) const override;
	int getSpawnResourcesAuxValue(int x) const override;

	void die(Level*, const TilePos& pos);

	int* m_checkBuffer;
	int field_74;
};
