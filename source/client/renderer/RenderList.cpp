/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "RenderList.hpp"
#include "Tesselator.hpp"

#include <cstddef>

constexpr int C_MAX_RENDERS = 3072;

RenderList::RenderList()
{
	field_14 = 0;
	m_bInited = false;
	m_bRendered = false;
	field_1C = 0;

	field_C = new int[C_MAX_RENDERS];
	field_10 = new RenderChunk[C_MAX_RENDERS];
}

RenderList::~RenderList()
{
	if (field_C)
		delete[] field_C;

	if (field_10)
		delete[] field_10;
}

void RenderList::add(int x)
{
	// @BUG: If too many chunks are rendered, this has the potential to overflow.
#ifndef ORIGINAL_CODE
	if (field_14 == C_MAX_RENDERS)
	{
		render();
		init(m_pos);
		field_1C = 0;
		m_bRendered = false;
	}
#endif

	field_C[field_14] = x;

	if (field_14 == C_MAX_RENDERS)
		render();
}

void RenderList::addR(RenderChunk& rc)
{
	// @BUG: If too many chunks are rendered, this has the potential to overflow.
#ifndef ORIGINAL_CODE
	if (field_14 == C_MAX_RENDERS)
	{
		render();
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

void RenderList::init(const Vec3& pos)
{
	m_pos = pos;
	field_14 = 0;
	m_bInited = true;
}

void RenderList::render()
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
		glPushMatrix();
		glTranslatef(-m_pos.x, -m_pos.y, -m_pos.z);
		renderChunks();
		glPopMatrix();
	}
}

void RenderList::renderChunks()
{
	if (field_1C > 0)
	{
		for (int i = 0; i < field_1C; i++)
		{
			RenderChunk& chk = field_10[i];

			glPushMatrix();

			glTranslatef(chk.m_pos.x, chk.m_pos.y, chk.m_pos.z);

			chk.render();

			glPopMatrix();
		}
	}
}
