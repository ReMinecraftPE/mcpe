/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "Frustum.hpp"

Frustum Frustum::frustum;

void Frustum::doOurJobInGameRenderer()
{
	Frustum& f = Frustum::frustum;

	f.m[16].fetchGL(GL_PROJECTION_MATRIX);
	f.m[17].fetchGL(GL_MODELVIEW_MATRIX);

	f.m[18] = f.m[17] * f.m[16];

	for (int i = 0; i < 6; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			f.m[i].c[j] = f.m[18].c[3 + j * 4] - f.m[18].c[i / 2 + j * 4];
		}

		f.normalizePlane(f.m, i);
	}
}

void Frustum::normalizePlane(Matrix* pMtxArr, int idx)
{
	Matrix& mtx = pMtxArr[idx];

	float x = Mth::invSqrt(mtx.c[1] * mtx.c[1] + mtx.c[0] * mtx.c[0] + mtx.c[2] * mtx.c[2]);

	mtx.c[0] *= x;
	mtx.c[1] *= x;
	mtx.c[2] *= x;
	mtx.c[3] *= x;
}

bool FrustumData::pointInFrustum(float x, float y, float z)
{
	for (int i = 0; i < 6; i++)
	{
		if ((this->x.m[i].c[0] * x) + (this->x.m[i].c[1] * y) + (this->x.m[i].c[2] * z) + this->x.m[i].c[3] <= 0.0f)
			return false;
	}
	return true;
}

bool FrustumData::sphereInFrustum(float x, float y, float z, float radius)
{
	for (int i = 0; i < 6; i++)
	{
		if ((this->x.m[i].c[0] * x) + (this->x.m[i].c[1] * y) + (this->x.m[i].c[2] * z) + this->x.m[i].c[3] <= (-radius))
			return false;
	}
	return true;
}

bool FrustumData::cubeFullyInFrustum(float x1, float y1, float z1, float x2, float y2, float z2)
{
	for (int i = 0; i < 6; i++) {
		if ((this->x.m[i].c[0] * x1) + (this->x.m[i].c[1] * y1) + (this->x.m[i].c[2] * z1) + this->x.m[i].c[3] <= 0.0f ||
			(this->x.m[i].c[0] * x2) + (this->x.m[i].c[1] * y1) + (this->x.m[i].c[2] * z1) + this->x.m[i].c[3] <= 0.0f ||
			(this->x.m[i].c[0] * x1) + (this->x.m[i].c[1] * y2) + (this->x.m[i].c[2] * z1) + this->x.m[i].c[3] <= 0.0f ||
			(this->x.m[i].c[0] * x2) + (this->x.m[i].c[1] * y2) + (this->x.m[i].c[2] * z1) + this->x.m[i].c[3] <= 0.0f ||
			(this->x.m[i].c[0] * x1) + (this->x.m[i].c[1] * y1) + (this->x.m[i].c[2] * z2) + this->x.m[i].c[3] <= 0.0f ||
			(this->x.m[i].c[0] * x2) + (this->x.m[i].c[1] * y1) + (this->x.m[i].c[2] * z2) + this->x.m[i].c[3] <= 0.0f ||
			(this->x.m[i].c[0] * x1) + (this->x.m[i].c[1] * y2) + (this->x.m[i].c[2] * z2) + this->x.m[i].c[3] <= 0.0f ||
			(this->x.m[i].c[0] * x2) + (this->x.m[i].c[1] * y2) + (this->x.m[i].c[2] * z2) + this->x.m[i].c[3] <= 0.0f)

			return false;
	}
	return true;
}

bool FrustumData::cubeInFrustum(float x1, float y1, float z1, float x2, float y2, float z2)
{
	for (int i = 0; i < 6; i++)
	{
		if ((this->x.m[i].c[0] * x1) + (this->x.m[i].c[1] * y1) + (this->x.m[i].c[2] * z1) + this->x.m[i].c[3] <= 0.0f &&
			(this->x.m[i].c[0] * x2) + (this->x.m[i].c[1] * y1) + (this->x.m[i].c[2] * z1) + this->x.m[i].c[3] <= 0.0f &&
			(this->x.m[i].c[0] * x1) + (this->x.m[i].c[1] * y2) + (this->x.m[i].c[2] * z1) + this->x.m[i].c[3] <= 0.0f &&
			(this->x.m[i].c[0] * x2) + (this->x.m[i].c[1] * y2) + (this->x.m[i].c[2] * z1) + this->x.m[i].c[3] <= 0.0f &&
			(this->x.m[i].c[0] * x1) + (this->x.m[i].c[1] * y1) + (this->x.m[i].c[2] * z2) + this->x.m[i].c[3] <= 0.0f &&
			(this->x.m[i].c[0] * x2) + (this->x.m[i].c[1] * y1) + (this->x.m[i].c[2] * z2) + this->x.m[i].c[3] <= 0.0f &&
			(this->x.m[i].c[0] * x1) + (this->x.m[i].c[1] * y2) + (this->x.m[i].c[2] * z2) + this->x.m[i].c[3] <= 0.0f &&
			(this->x.m[i].c[0] * x2) + (this->x.m[i].c[1] * y2) + (this->x.m[i].c[2] * z2) + this->x.m[i].c[3] <= 0.0f)

			return false;
	}
	return true;
}

bool FrustumData::cubeInFrustum(const AABB& aabb)
{
	return cubeInFrustum(aabb.min.x, aabb.min.y, aabb.min.z, aabb.max.x, aabb.max.y, aabb.max.z);
}
