#pragma once

#include "Matrix.hpp"
#include "AABB.hpp"

class Frustum
{
public:
	void normalizePlane(Matrix*, int);

	//it's inlined in GameRenderer
	static void doOurJobInGameRenderer();

	static Frustum frustum;

public:
	Matrix m[19];
};

class FrustumData
{
public:
	bool pointInFrustum(float x, float y, float z);
	bool sphereInFrustum(float x, float y, float z, float radius);
	bool cubeFullyInFrustum(float x1, float y1, float z1, float x2, float y2, float z2);
	bool cubeInFrustum(float x1, float y1, float z1, float x2, float y2, float z2);
	bool cubeInFrustum(const AABB& aabb);

public:
	Frustum x;
};