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
	static const Color DEFAULT_COLOR;

public:
	LeafTile(TileID id);
	~LeafTile();

private:
	void _tickDecayOld(TileSource* level, const TilePos& pos); // circa 0.1.0
	void _tickDecay(TileSource* level, const TilePos& pos); /// circa b1.7.3

public:
	int getColor(TileSource*, const TilePos& pos) const override;
	int getTexture(Facing::Name face, TileData data) const override;
	bool isSolidRender() const override;
	void onRemove(TileSource*, const TilePos& pos) override;
	void stepOn(TileSource*, const TilePos& pos, Entity*) override;
	void tick(TileSource*, const TilePos& pos, Random*) override;
	int getResource(TileData data, Random* random) const override;
	int getSpawnResourcesAuxValue(int x) const override;

	void die(TileSource*, const TilePos& pos);
	
	static bool isDeepLeafTile(TileSource&, const TilePos&);

public:
	int* m_checkBuffer;
	int field_74;
};
