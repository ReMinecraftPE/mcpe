/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "IArea.hpp"

class PolygonArea : public IArea
{
public:
	PolygonArea(int count, const float* x, const float* y);
	~PolygonArea();

	bool isInside(float x, float y) override;

public:
	float* m_xPos;
	float* m_yPos;
	int m_count;
};

