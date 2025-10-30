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
	m_posX = 0.0f;
	m_posY = 0.0f;
	m_posZ = 0.0f;
	field_14 = 0;
	field_18 = false;
	field_19 = false;
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
		init(m_posX, m_posY, m_posZ);
		field_1C = 0;
		field_19 = false;
	}
#endif

	field_C[field_14] = x;

	if (field_14 == C_MAX_RENDERS)
		render();
}

void RenderList::addR(const RenderChunk& rc)
{
	// @BUG: If too many chunks are rendered, this has the potential to overflow.
#ifndef ORIGINAL_CODE
	if (field_14 == C_MAX_RENDERS)
	{
		render();
		init(m_posX, m_posY, m_posZ);
		field_1C = 0;
		field_19 = false;
	}
#endif

	field_10[field_14] = rc;
}

void RenderList::clear()
{
	field_18 = false;
	field_19 = false;
}

void RenderList::init(float x, float y, float z)
{
	m_posX = x;
	m_posY = y;
	m_posZ = z;
	field_14 = 0;
	field_18 = true;
}

void RenderList::render()
{
	if (!field_18) return;

	if (!field_19)
	{
		field_19 = true;
		field_1C = field_14;
		field_14 = 0;
	}

	if (field_14 < field_1C)
	{
		glPushMatrix();
		glTranslatef(-m_posX, -m_posY, -m_posZ);
		renderChunks();
		glPopMatrix();
	}
}

void RenderList::renderChunks()
{
	xglEnableClientState(GL_VERTEX_ARRAY);
	xglEnableClientState(GL_COLOR_ARRAY);
	xglEnableClientState(GL_TEXTURE_COORD_ARRAY);

	if (field_1C > 0)
	{
		for (int i = 0; i < field_1C; i++)
		{
			RenderChunk& chk = field_10[i];
			glPushMatrix();

			glTranslatef(chk.field_C, chk.field_10, chk.field_14);
			xglBindBuffer(GL_ARRAY_BUFFER, chk.field_0);
			xglVertexPointer  (3, GL_FLOAT,         sizeof(Tesselator::Vertex), (void*)offsetof(Tesselator::Vertex, m_x));
			xglTexCoordPointer(2, GL_FLOAT,         sizeof(Tesselator::Vertex), (void*)offsetof(Tesselator::Vertex, m_u));
			xglColorPointer   (4, GL_UNSIGNED_BYTE, sizeof(Tesselator::Vertex), (void*)offsetof(Tesselator::Vertex, m_color));
			xglDrawArrays(GL_TRIANGLES, 0, chk.field_4);

			glPopMatrix();
		}
	}

	xglDisableClientState(GL_VERTEX_ARRAY);
	xglDisableClientState(GL_COLOR_ARRAY);
	xglDisableClientState(GL_TEXTURE_COORD_ARRAY);
}
