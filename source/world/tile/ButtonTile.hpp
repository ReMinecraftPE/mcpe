/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "Tile.hpp"

class ButtonTile : public Tile
{
public:
	ButtonTile(int id, int texture);
	
	AABB* getAABB(Level*, int x, int y, int z) override;
	int getTickDelay() override;
	bool isSolidRender() override;
	bool isCubeShaped() override;
	bool mayPlace(Level*, int, int, int) override;
	void onPlace(Level*, int, int, int) override;
	void setPlacedOnFace(Level*, int, int, int, int) override;
	void neighborChanged(Level*, int, int, int, int) override;
	void updateShape(LevelSource*, int, int, int) override;
	int use(Level*, int, int, int, Player*);
	void tick(Level*, int, int, int, Random*);
	void destroy(Level*, int, int, int, int) override;
	int getSignal(LevelSource*, int, int, int, int) override;
	int getDirectSignal(LevelSource*, int, int, int, int) override;
	bool isSignalSource() override;
	void updateDefaultShape() override;

	bool checkCanSurvive(Level*, int, int, int);
};
