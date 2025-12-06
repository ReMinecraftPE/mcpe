/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "RenderChunk.hpp"
#include "world/phys/Vec3.hpp"

class RenderList
{
public:
	RenderList();
	~RenderList();

	void add(int x, TerrainLayer layer, bool fog);
	void addR(RenderChunk* rc, TerrainLayer layer, bool fog);
	void clear();
	void reset();
	void init(const Vec3& pos);
	void render(TerrainLayer layer, bool fog);
	void renderChunks(TerrainLayer layer, bool fog);

private:
	Vec3 m_pos;
	int* field_C;
	RenderChunk** field_10;
	int field_14;
	bool m_bInited;
	bool m_bRendered;
	int field_1C;
};

