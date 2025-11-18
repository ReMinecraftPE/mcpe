/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "thirdparty/GL/GL.hpp"
#include "renderer/Mesh.hpp"
#include "world/level/TilePos.hpp"

class RenderChunk
{
public:
	TilePos m_pos;
	mce::Mesh m_mesh;

private:
	void _init();

public:
	RenderChunk() { _init(); }
	RenderChunk(const TilePos& pos, mce::Mesh& mesh);
	
public:
	void render();
	void reset();
};

