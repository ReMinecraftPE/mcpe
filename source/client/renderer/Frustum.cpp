/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "Frustum.hpp"
#include "GL/GL.hpp"
#include "thirdparty/glm/gtc/matrix_access.hpp"
#include "thirdparty/glm/gtc/type_ptr.hpp"

Frustum Frustum::frustum;

void Frustum::calculateFrustum()
{
	Frustum& f = Frustum::frustum;

#ifdef ENH_GFX_MATRIX_STACK
	const Matrix& projMtx = MatrixStack::Projection.top();
	const Matrix& worldMtx = MatrixStack::World.top();
	const Matrix& viewMtx = MatrixStack::View.top();
	// Order matters!
	Matrix worldViewProjMtx = projMtx * viewMtx * worldMtx;
#else
	Matrix projMtx, viewMtx;
	glGetFloatv(GL_PROJECTION_MATRIX, projMtx.getPtr()); // _proj
	glGetFloatv(GL_MODELVIEW_MATRIX, viewMtx.getPtr()); // _modl
	// Order matters!
	Matrix worldViewProjMtx = projMtx * viewMtx;
#endif

	glm::mat4& mtx = worldViewProjMtx._m;

	const glm::vec4 row0 = glm::row(mtx, 0); // X
	const glm::vec4 row1 = glm::row(mtx, 1); // Y
	const glm::vec4 row2 = glm::row(mtx, 2); // Z
	const glm::vec4 row3 = glm::row(mtx, 3); // W

	f.m[0] = row3 - row0; // Right
	f.m[1] = row3 + row0; // Left
	f.m[2] = row3 + row1; // Bottom
	f.m[3] = row3 - row1; // Top
	f.m[4] = row3 - row2; // Far
	f.m[5] = row3 + row2; // Near

	for (int i = 0; i < 6; ++i)
	{
		glm::vec4& vec = f.m[i];
		f.m[i] = glm::normalize(vec);
	}
}

bool FrustumData::pointInFrustum(const Vec3& vec)
{
	for (int i = 0; i < 6; i++)
	{
		float* mtx = glm::value_ptr(this->x.m[i]);
		if ((mtx[0] * vec.x) + (mtx[1] * vec.y) + (mtx[2] * vec.z) + mtx[3] <= 0.0f)
			return false;
	}
	return true;
}

bool FrustumData::sphereInFrustum(const Vec3& vec, float radius)
{
	for (int i = 0; i < 6; i++)
	{
		float* mtx = glm::value_ptr(this->x.m[i]);
		if ((mtx[0] * vec.x) + (mtx[1] * vec.y) + (mtx[2] * vec.z) + mtx[3] <= (-radius))
			return false;
	}
	return true;
}

bool FrustumData::cubeInFrustum(const Vec3& min, const Vec3& max)
{
	for (int i = 0; i < 6; i++)
	{
		const glm::vec4& plane = this->x.m[i];
		glm::vec3 normal = glm::vec3(plane); // The A, B, C components

		// Find the "Positive Vertex" (P-Vertex).
		// This is the corner of the box that is furthest along the normal's direction.
		glm::vec3 pVertex;
		pVertex.x = (normal.x > 0) ? max.x : min.x;
		pVertex.y = (normal.y > 0) ? max.y : min.y;
		pVertex.z = (normal.z > 0) ? max.z : min.z;

		// Check distance from plane to the P-Vertex.
		// Plane Equation: Ax + By + Cz + D = 0
		// If (dot(n, p) + d) < 0, the point is behind the plane.
		// If the "furthest" point is behind the plane, the whole box is outside.
		if (glm::dot(normal, pVertex) + plane.w < 0.0f)
		{
			return false;
		}
	}
	return true;
}

bool FrustumData::cubeInFrustum(const AABB& aabb)
{
	return cubeInFrustum(aabb.min, aabb.max);
}
