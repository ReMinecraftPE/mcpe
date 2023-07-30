/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "LightLayer.hpp"

class Level;

struct LightUpdate
{
	const LightLayer* m_lightLayer;
	int m_x1, m_y1, m_z1, m_x2, m_y2, m_z2;

	LightUpdate(const LightLayer& ll, int x1, int y1, int z1, int x2, int y2, int z2)
	{
		m_lightLayer = &ll;
		m_x1 = x1; m_x2 = x2;
		m_y1 = y1; m_y2 = y2;
		m_z1 = z1; m_z2 = z2;
	}

	void update(Level* pLevel);
	bool expandToContain(int x1, int y1, int z1, int x2, int y2, int z2);
};

