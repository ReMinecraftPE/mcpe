/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "Tile.hpp"

// not an actual name.
class WireTile : public Tile
{
public:
	// Use as (1 << CONN_??).
	enum
	{
		CONN_XN,
		CONN_XP,
		CONN_ZN,
		CONN_ZP,
		CONN_COUNT,

		CONN_ABOVE_XN = 4,
		CONN_ABOVE_XP,
		CONN_ABOVE_ZN,
		CONN_ABOVE_ZP,

		CONN_MASK       = 0x0F, // mask in only the connection directions, not if any of them are going above.
		CONN_ABOVE_MASK = 0xF0, // mask in only whether any of the directions go up
	};

	WireTile(int ID);

	bool isSolidRender() override;
	bool isCubeShaped() override;
	int getRenderShape() override;
	bool isSignalSource() override;
	void updateShape(LevelSource*, int x, int y, int z) override;
	AABB* getAABB(Level*, int x, int y, int z) override;
	AABB getTileAABB(Level*, int x, int y, int z) override;
	void addAABBs(Level*, int x, int y, int z, const AABB* aabb, std::vector<AABB>& out) override;

	int getConnections(LevelSource*, int x, int y, int z);

private:
	bool m_bIsPowerSource;
};
