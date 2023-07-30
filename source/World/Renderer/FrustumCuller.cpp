/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "FrustumCuller.hpp"

bool FrustumCuller::cubeFullyInFrustum(float minX, float minY, float minZ, float maxX, float maxY, float maxZ)
{
	return m_frustumData.cubeFullyInFrustum(
		minX - m_camPos.x,
		minY - m_camPos.y,
		minZ - m_camPos.z,
		maxX - m_camPos.x,
		maxY - m_camPos.y,
		maxZ - m_camPos.z
	);
}

bool FrustumCuller::cubeInFrustum(float minX, float minY, float minZ, float maxX, float maxY, float maxZ)
{
	return m_frustumData.cubeInFrustum(
		minX - m_camPos.x,
		minY - m_camPos.y,
		minZ - m_camPos.z,
		maxX - m_camPos.x,
		maxY - m_camPos.y,
		maxZ - m_camPos.z
	);
}

bool FrustumCuller::isVisible(const AABB& aabb)
{
	return cubeInFrustum(aabb.min.x, aabb.min.y, aabb.min.z, aabb.max.x, aabb.max.y, aabb.max.z);
}

void FrustumCuller::prepare(float x, float y, float z)
{
	m_camPos = Vec3(x, y, z);
}
