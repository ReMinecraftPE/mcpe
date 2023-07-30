/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

struct VertexPT
{
	float x = 0, y = 0, z = 0;
	float u = 0, v = 0;

	VertexPT() {}
	VertexPT(float x, float y, float z) : x(x), y(y), z(z) {}
	VertexPT(float x, float y, float z, float(u), float(v)) : x(x), y(y), z(z), u(u), v(v) {}

	void setUV(float _u, float _v)
	{
		u = _u, v = _v;
	}
};
