/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include <set>
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
	int getTickDelay() override;
	bool isSignalSource() override;
	bool canSurvive(Level*, int x, int y, int z) override;
	void updateShape(LevelSource*, int x, int y, int z) override;
	AABB* getAABB(Level*, int x, int y, int z) override;
	AABB getTileAABB(Level*, int x, int y, int z) override;
	void addAABBs(Level*, int x, int y, int z, const AABB* aabb, std::vector<AABB>& out) override;
	void onPlace(Level*, int x, int y, int z) override;
	void onRemove(Level*, int x, int y, int z) override;
	void neighborChanged(Level*, int x, int y, int z, int id) override;
	int getSignal(LevelSource*, int x, int y, int z, int dir) override;
	int getDirectSignal(LevelSource*, int x, int y, int z, int dir) override;
	int getColor(LevelSource*, int, int, int) override;

	bool isSignalSource(LevelSource*, int x, int y, int z);
	int getConnections(LevelSource*, int x, int y, int z);
	void recalculate(Level* level, int x, int y, int z);
	void calculateChanges(Level* level, int x, int y, int z, int x2, int y2, int z2);
	int getStrongerSignal(Level* level, int x, int y, int z, int prevSignal);
	void updateWires(Level* level, int x, int y, int z);

private:
	bool m_bIsPowerSource;

	std::set<TilePos> m_positionsToUpdate;
};
