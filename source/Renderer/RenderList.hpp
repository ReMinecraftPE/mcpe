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
	float m_posX = 0.0f;
	float m_posY = 0.0f;
	float m_posZ = 0.0f;
	int* field_C = nullptr;
	RenderChunk* field_10 = nullptr;
	int field_14 = 0;
	bool field_18 = false;
	bool field_19 = false;
	int field_1C = 0;
};

