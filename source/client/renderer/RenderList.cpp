/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "RenderList.hpp"
#include "renderer/GlobalConstantBuffers.hpp"
#include "renderer/MatrixStack.hpp"

constexpr int C_MAX_RENDERS = 3072;

RenderList::RenderList()
{
	field_14 = 0;
	m_bInited = false;
	m_bRendered = false;
	field_1C = 0;

	field_C = new int[C_MAX_RENDERS];
	field_10 = new RenderChunk*[C_MAX_RENDERS];
}

RenderList::~RenderList()
{
	if (field_C)
		delete[] field_C;

	if (field_10)
		delete[] field_10;
}

void RenderList::add(int x, TerrainLayer layer, bool fog)
{
	// @BUG: If too many chunks are rendered, this has the potential to overflow.
#ifndef ORIGINAL_CODE
	if (field_14 == C_MAX_RENDERS)
	{
		render(layer, fog);
		init(m_pos);
		field_1C = 0;
		m_bRendered = false;
	}
#endif

	field_C[field_14] = x;

	if (field_14 == C_MAX_RENDERS)
		render(layer, fog);
}

void RenderList::addR(RenderChunk* rc, TerrainLayer layer, bool fog)
{
	// @BUG: If too many chunks are rendered, this has the potential to overflow.
#ifndef ORIGINAL_CODE
	if (field_14 == C_MAX_RENDERS)
	{
		render(layer, fog);
		init(m_pos);
		field_1C = 0;
		m_bRendered = false;
	}
#endif

	field_10[field_14] = rc;

	field_14++;
}

void RenderList::clear()
{
	m_bInited = false;
	m_bRendered = false;
}

void RenderList::reset()
{
	for (int i = 0; i < field_1C; i++)
	{
		RenderChunk* chk = field_10[i];
		if (!chk) continue;
		
		chk->reset();
	}
}

void RenderList::init(const Vec3& pos)
{
	m_pos = pos;
	field_14 = 0;
	m_bInited = true;
}

void RenderList::render(TerrainLayer layer, bool fog)
{
	if (!m_bInited) return;

	if (!m_bRendered)
	{
		m_bRendered = true;
		field_1C = field_14;
		field_14 = 0;
	}

	if (field_14 < field_1C)
	{
		MatrixStack::Ref matrix = MatrixStack::World.push();
		matrix->translate(-m_pos);

		renderChunks(layer, fog);
	}
}

void RenderList::renderChunks(TerrainLayer layer, bool fog)
{
	double time = getTimeS();

	if (field_1C > 0)
	{
		for (int i = 0; i < field_1C; i++)
		{
			RenderChunk* chk = field_10[i];
			if (!chk) continue;

			MatrixStack::Ref matrix = MatrixStack::World.push();
			matrix->translate(chk->m_pos);

#ifdef FEATURE_GFX_SHADERS
			mce::GlobalConstantBuffers& globalBuffers = mce::GlobalConstantBuffers::getInstance();
			mce::RenderChunkConstants& chunkConstants = globalBuffers.m_renderChunkConstants;

			Vec3 chunkOrigin = chk->m_pos;
			chunkConstants.CHUNK_ORIGIN->setData(&chunkOrigin);

			chunkConstants.sync();
#endif

			chk->render(layer, time, fog);
		}
	}
}
