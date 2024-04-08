/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "Tile.hpp"

class DiodeTile : public Tile
{
public:
	DiodeTile(int id, bool on);
	
	bool isCubeShaped() override;
	bool mayPlace(Level*, int, int, int) override;
	bool canSurvive(Level*, int, int, int) override;
	void tick(Level*, int, int, int, Random*);
	int getTexture(int dir, int data) override;
	int getTexture(int dir) override;
	bool shouldRenderFace(LevelSource*, int, int, int, int) override;
	int getRenderShape() override;
	int getSignal(LevelSource*, int, int, int, int) override;
	int getDirectSignal(LevelSource*, int, int, int, int) override;
	void neighborChanged(Level*, int, int, int, int) override;
	int use(Level*, int, int, int, Player*) override;
	bool isSignalSource() override;
	void setPlacedBy(Level*, int, int, int, Mob*) override;
	void onPlace(Level*, int, int, int) override;
	bool isSolidRender() override;
	int getResource(int, Random*) override;
	void animateTick(Level*, int, int, int, Random*) override;

	bool hasSignal(Level*, int, int, int, int);

private:
	bool m_bActive;

public:
	static int m_delays[];
	static float m_particleOffsets[];
};
