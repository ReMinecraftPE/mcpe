/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "world/phys/AABB.hpp"

class Culler
{
public:
	virtual ~Culler();
	virtual bool isVisible(const AABB&) = 0;
	virtual bool cubeInFrustum(float, float, float, float, float, float) = 0;
	virtual bool cubeFullyInFrustum(float, float, float, float, float, float) = 0;
	virtual void prepare(float, float, float);
};

