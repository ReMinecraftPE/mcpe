/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "Tile.hpp"

class DoorTile : public Tile
{
public:
	DoorTile(int ID, Material*);

	void attack(Level*, const TilePos& pos, Player*) override;
	bool use(Level*, const TilePos& pos, Player*) override;
	HitResult clip(const Level*, const TilePos& pos, Vec3, Vec3) override;
	AABB* getAABB(const Level*, const TilePos& pos) override;
	int getRenderShape() const override;
	int getResource(TileData data, Random*) const override;
	int getTexture(Facing::Name face, TileData data) const override;
	AABB getTileAABB(const Level*, const TilePos& pos) override;
	bool isCubeShaped() const override;
	bool isSolidRender() const override;
	bool mayPlace(const Level*, const TilePos& pos) const override;
	void updateShape(const LevelSource*, const TilePos& pos) override;
	void neighborChanged(Level*, const TilePos& pos, TileID newTile) override;

	bool blocksLight() const;
	int getDir(TileData data) const;
	void setOpen(Level*, const TilePos& pos, bool bOpen);
	
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Woverloaded-virtual"
	void setShape(int dir);
#pragma GCC diagnostic pop

	// @NOTE: These are inlined.
	inline static bool isOpen(TileData data)
	{
		return (data & 4) != 0;
	}
	inline static bool isTop(TileData data)
	{
		return (data & 8) != 0;
	}
};
