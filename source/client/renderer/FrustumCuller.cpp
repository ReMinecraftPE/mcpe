/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "FrustumCuller.hpp"

bool FrustumCuller::cubeInFrustum(const Vec3& min, const Vec3& max)
{
	return m_frustumData.cubeInFrustum(min - m_camPos, max - m_camPos);
}

bool FrustumCuller::isVisible(const AABB& aabb)
{
	return cubeInFrustum(aabb.min, aabb.max);
}

void FrustumCuller::prepare(const Vec3& camPos)
{
	m_camPos = camPos;
}
