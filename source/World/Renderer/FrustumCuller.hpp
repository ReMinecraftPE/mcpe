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

