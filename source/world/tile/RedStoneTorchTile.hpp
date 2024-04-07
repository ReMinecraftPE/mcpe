/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include <list>
#include "TorchTile.hpp"
#include "TorchUpdateEvent.hpp"

class RedStoneTorchTile : public TorchTile
{
public:
	RedStoneTorchTile(int id, int texture, Material* pMtl);

	bool isSignalSource() override;
	int getTickDelay() override;
	void onPlace(Level*, int x, int y, int z) override;
	void onRemove(Level*, int x, int y, int z) override;
	void neighborChanged(Level*, int x, int y, int z, int dir) override;
	int getResource(int data, Random* random) override;
	void tick(Level*, int, int, int, Random*) override;
	int getSignal(LevelSource*, int x, int y, int z, int dir) override;
	int getDirectSignal(LevelSource*, int x, int y, int z, int dir) override;
	void animateTick(Level* level, int x, int y, int z, Random* random) override;

	void updateNeighbors(Level*, int x, int y, int z, int id);
	bool checkBurnOut(Level*, int x, int y, int z, bool b);
	bool hasSignalFromBehind(Level*, int x, int y, int z);

	bool isActive() {
		return m_bActive;
	}
private:
	bool m_bActive;
	static TorchUpdateEvents m_recentUpdates;
};

