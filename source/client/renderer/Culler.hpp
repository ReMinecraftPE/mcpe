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
	virtual bool isVisible(const AABB& aabb) = 0;
	virtual bool cubeInFrustum(const Vec3& min, const Vec3& max) = 0;
	virtual void prepare(const Vec3& camPos);
};

