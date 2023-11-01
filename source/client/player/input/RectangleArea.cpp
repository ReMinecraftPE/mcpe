/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "RectangleArea.hpp"

RectangleArea::RectangleArea(float _left, float _top, float _right, float _bottom) :
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
