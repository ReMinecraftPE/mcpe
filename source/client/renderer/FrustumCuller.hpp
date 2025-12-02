/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "world/phys/Vec3.hpp"
#include "Culler.hpp"
#include "Frustum.hpp"

class FrustumCuller : public Culler
{
public:
	bool cubeInFrustum(const Vec3& min, const Vec3& max);
	bool isVisible(const AABB& aabb);
	void prepare(const Vec3& camPos);

public:
	FrustumData m_frustumData;
	Vec3 m_camPos;
};

