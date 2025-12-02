/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "renderer/MatrixStack.hpp"
#include "world/phys/AABB.hpp"

class Frustum
{
public:
	//it's inlined in GameRenderer
	static void calculateFrustum();

	static Frustum frustum;

public:
	glm::vec4 m[6];
};

class FrustumData
{
public:
	bool pointInFrustum(const Vec3& vec);
	bool sphereInFrustum(const Vec3& vec, float radius);
	bool cubeInFrustum(const Vec3& min, const Vec3& max);
	bool cubeInFrustum(const AABB& aabb);

public:
	Frustum x;
};