#pragma once

#include "AABB.hpp"

class Culler
{
public:
	virtual ~Culler();
	virtual bool isVisible(const AABB&) = 0;
	virtual bool cubeInFrustum(float, float, float, float, float, float) = 0;
	virtual bool cubeFullyInFrustum(float, float, float, float, float, float) = 0;
	virtual void prepare(float, float, float);
};

