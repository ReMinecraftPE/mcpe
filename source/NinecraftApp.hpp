/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "Minecraft.hpp"
#include "world/level/Level.hpp"
#include "world/tile/Tile.hpp"

//@TYPO: This is probably meant to say "MinecraftApp". Still not fixed in V0.3.0 though so not sure
class NinecraftApp : public Minecraft
{
public:
	virtual ~NinecraftApp();

	bool handleBack(bool) override;
	void init() override;
	void update() override;
	void onGraphicsReset() override;
	void teardown();

	void updateStats();
	void initGLStates();

private:
	int field_DBC = 0;
	bool field_DC0 = 1;
	int m_fps = 0;

	static bool _hasInitedStatics;
};

