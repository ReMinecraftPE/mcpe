/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "Vec3.hpp"
#include "Culler.hpp"
#include "Frustum.hpp"

class FrustumCuller : public Culler
{
public:
	bool cubeFullyInFrustum(float, float, float, float, float, float);
	bool cubeInFrustum(float, float, float, float, float, float);
	bool isVisible(const AABB&);
	void prepare(float x, float y, float z);

public:
	FrustumData m_frustumData;
	Vec3 m_camPos;
};

