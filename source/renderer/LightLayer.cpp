/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "LightLayer.hpp"

LightLayer LightLayer::Sky(15), LightLayer::Block(0);

LightLayer::LightLayer(int x)
{
	m_x = x;
}
