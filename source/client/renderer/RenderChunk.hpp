/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "compat/GL.hpp"

class RenderChunk
{
	static int runningId;

public:
	GLuint field_0 = -1;
	int field_4 = 0;
	int m_id;
	float field_C = 0.0f;
	float field_10 = 0.0f;
	float field_14 = 0.0f;

public:
	RenderChunk()
	{
		m_id = ++runningId;
	}
	RenderChunk(GLuint a1, int a2)
	{
		m_id = ++runningId;
		field_0 = a1;
		field_4 = a2;
	}
};

