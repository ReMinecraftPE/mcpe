/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "LightLayer.hpp"
#include "world/level/TilePos.hpp"

class Level;

struct LightUpdate
{
	const LightLayer* m_lightLayer;
	TilePos m_tilePos1, m_tilePos2;

	LightUpdate(const LightLayer& ll, const TilePos& tilePos1, const TilePos& tilePos2)
	{
		m_lightLayer = &ll;
		m_tilePos1 = tilePos1;
		m_tilePos2 = tilePos2;
	}

	void update(Level* pLevel);
	bool expandToContain(const TilePos& tilePos1, const TilePos& tilePos2);
};

