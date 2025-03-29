/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "Entity.hpp"

class FallingTile : public Entity
{
private:
	void _init(TileID tileId = TILE_AIR, int tileData = 0);
	void _init(const Vec3& pos, TileID tileId, int tileData);
public:
	FallingTile(Level* level) : Entity(level) { _init(); }
	FallingTile(Level* level, const Vec3& pos, TileID tileId, int tileData = 0) : Entity(level) { _init(pos, tileId, tileData); };

	float getShadowHeightOffs() const override;
	bool isPickable() const override;
	void tick() override;

	Level* getLevel();

public:
	TileID m_tileId;
	int m_tileData;
	int m_time;
};

