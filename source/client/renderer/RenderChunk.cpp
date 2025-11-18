/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "RenderChunk.hpp"

void RenderChunk::_init()
{
}

RenderChunk::RenderChunk(const TilePos& pos, mce::Mesh& mesh)
	: m_pos(pos)
	, m_mesh(mesh)
{
	_init();
}

void RenderChunk::render()
{
	m_mesh.render(mce::MaterialPtr::NONE);
}

void RenderChunk::reset()
{
	m_mesh.reset();
}