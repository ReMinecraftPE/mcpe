#pragma once

#include "IArea.hpp"

class RectangleArea : public IArea
{
public:
	RectangleArea(float _left, float _right, float _top, float _bottom);
	bool isInside(float x, float y) override;

private:
	float left;
	float right;
	float top;
	float bottom;
};

