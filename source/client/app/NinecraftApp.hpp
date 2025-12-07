/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "client/app/Minecraft.hpp"
#include "world/level/Level.hpp"
#include "world/tile/Tile.hpp"

//@TYPO: This is probably meant to say "MinecraftApp". Still not fixed in V0.3.0 though so not sure
// This is renamed to MinecraftClient in 0.13.1
class NinecraftApp : public Minecraft
{
public:
	NinecraftApp();
	virtual ~NinecraftApp();

private:
	void _initOptions();
	void _initTextures();
	void _initMaterials();
	void _initInput();
	void _updateStats();

protected:
	void _reloadTextures();
	void _reloadFancy(bool isFancy);
	void _reloadOptionalFeatures();
	void _reloadPatchData();

public:
	bool handleBack(bool) override;
	void init() override;
	void setupRenderer();
	void update() override;
	void onGraphicsReset();
	void teardown();
	void teardownRenderer();
	void reloadFancy(bool isFancy) override;

	int getFpsIntlCounter() override;

private:
	int field_DBC;
	bool field_DC0;
	int m_fps;

	static bool _hasInitedStatics;
};

