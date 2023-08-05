/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "RenderChunk.hpp"

class RenderList
{
public:
	RenderList();
	~RenderList();

	void add(int x);
	void addR(const RenderChunk&);
	void clear();
	void init(float, float, float);
	void render();
	void renderChunks();

public:
	float m_posX;
	float m_posY;
	float m_posZ;
	int* field_C;
	RenderChunk* field_10;
	int field_14;
	bool field_18;
	bool field_19;
	int field_1C;
};

