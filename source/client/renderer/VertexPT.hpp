/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "world/phys/Vec3.hpp"

struct VertexPT
{
	Vec3 pos;
	float u , v ;

	VertexPT()
	{
		pos = Vec3::ZERO;
        u = 0; v = 0;
	}
	VertexPT(float x, float y, float z, float(u), float(v)) : pos(Vec3(x,y,z)), u(u), v(v) {}
	VertexPT(Vec3& pos) : pos(pos), u(0), v(0) {}

	void setUV(float _u, float _v)
	{
        u = _u; v = _v;
	}
};
