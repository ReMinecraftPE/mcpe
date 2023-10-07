#pragma once

#include "IArea.hpp"

class PolygonArea : public IArea
{
public:
	PolygonArea(int count, const float* x, const float* y);
	~PolygonArea();

	bool isInside(float x, float y) override;

private:
	float* m_xPos;
	float* m_yPos;
	int m_count;
};

