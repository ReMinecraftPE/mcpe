#include "RectangleArea.hpp"

RectangleArea::RectangleArea(float _left, float _right, float _top, float _bottom) :
	left(_left),
	right(_right),
	top(_top),
	bottom(_bottom)
{
}

bool RectangleArea::isInside(float x, float y)
{
	return left   <= x &&
	       right  >= x &&
	       top    <= y &&
	       bottom >= y;
}
