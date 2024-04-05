/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "Tesselator.hpp"
#include "thirdparty/GL/GL.hpp"
#include "common/Utils.hpp"

#include <cstddef>

int dword_2514A4 = 0;

int g_nVertices = 0, g_nTriangles = 0;

Tesselator Tesselator::instance;

Tesselator::Tesselator(int allotedSize)
{
	m_pVertices = nullptr;
	field_4 = 0;
	m_offsetX = 0.0f;
	m_offsetY = 0.0f;
	m_offsetZ = 0.0f;

	m_nextVtxU = 0;
	m_nextVtxV = 0;
	m_nextVtxColor = 0;

	m_bHaveColor = false;
	m_bHaveTex = false;
	field_26 = false;
	m_bBlockColor = false;
	field_28 = false;
	m_nVertices = 0;
	field_30 = 0;
	field_34 = false;

	m_vboCount = 1024;
	field_3C = -1;
	m_pVBOs = nullptr;

	field_48 = 0;
	m_maxVertices = 0;
	m_drawArraysMode = 0;
	m_accessMode = 0;

	m_maxVertices = allotedSize / sizeof(Vertex);
	m_accessMode = 2;

	m_pVBOs = new GLuint[m_vboCount];
	m_pVertices = new Vertex[m_maxVertices];
}

Tesselator::~Tesselator()
{
	if (m_pVBOs)
		delete[] m_pVBOs;
	if (m_pVertices)
		delete[] m_pVertices;
}

void Tesselator::addOffset(float x, float y, float z)
{
	m_offsetX += x;
	m_offsetY += y;
	m_offsetZ += z;
}

void Tesselator::clear()
{
	m_accessMode = 2;
	field_4 = 0;
	field_30 = 0;
	m_nVertices = 0;
	field_28 = 0;
}

void Tesselator::color(int r, int g, int b, int a)
{
	if (m_bBlockColor) return;

	if (b >= 255) b = 255;
	if (g >= 255) g = 255;
	if (r >= 255) r = 255;
	if (a >= 255) a = 255;

	// note: weird bit hacking (as in X & ~(X >> 31)) is used to perform the following:
	if (b < 0) b = 0;
	if (g < 0) g = 0;
	if (r < 0) r = 0;
	if (a < 0) a = 0;

	m_bHaveColor = true;
	m_nextVtxColor = a << 24 | b << 16 | g << 8 | r;
}

void Tesselator::color(int r, int g, int b)
{
	color(r, g, b, 255);
}

void Tesselator::color(int c, int a)
{
	color((c >> 16) & 0xFF, (c >> 8) & 0xFF, c & 0xFF, a);
}

void Tesselator::color(int c)
{
	color((c >> 16) & 0xFF, (c >> 8) & 0xFF, c & 0xFF, 255);
}

void Tesselator::color(char r, char g, char b)
{
	color(int(r), int(g), int(b));
}

void Tesselator::color(float r, float g, float b)
{
	color(r, g, b, 1.0f);
}

void Tesselator::color(float r, float g, float b, float a)
{
	color(int(r * 255), int(g * 255), int(b * 255), int(a * 255));
}

void Tesselator::begin()
{
	begin(GL_QUADS);
}

void Tesselator::begin(int drawMode)
{
	if (field_34 || field_28) return;

	field_34 = true;
	clear();
	m_drawArraysMode = drawMode;
	field_26 = 0;
	m_bHaveColor  = false;
	m_bHaveTex    = false;
	m_bBlockColor = false;
}

void Tesselator::draw()
{
	if (!field_34) return;
	if (field_28) return;

	field_34 = 0;

	if (field_4 > 0)
	{
		field_3C++;
		if (field_3C >= m_vboCount)
			field_3C = 0;

		xglBindBuffer(GL_ARRAY_BUFFER, m_pVBOs[field_3C]);
		xglBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * m_nVertices, m_pVertices, m_accessMode == 1 ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);

		if (m_bHaveTex)
		{
			// it won't use address 12, because we've bound a buffer object
			xglTexCoordPointer(2, GL_FLOAT, sizeof(Vertex), (void*)offsetof(Vertex, m_u));
			xglEnableClientState(GL_TEXTURE_COORD_ARRAY);
		}
		if (m_bHaveColor)
		{
			// it won't use address 12, because we've bound a buffer object
			xglColorPointer(4, GL_UNSIGNED_BYTE, sizeof(Vertex), (void*)offsetof(Vertex, m_color));
			xglEnableClientState(GL_COLOR_ARRAY);
		}

		xglVertexPointer(3, GL_FLOAT, sizeof(Vertex), (void*)offsetof(Vertex, m_x));
		xglEnableClientState(GL_VERTEX_ARRAY);

		// if we want to draw quads, draw triangles actually
		// otherwise, just pass the mode, it's fine
		if (m_drawArraysMode == GL_QUADS)
			xglDrawArrays(GL_TRIANGLES, 0, field_4);
		else
			xglDrawArrays(m_drawArraysMode, 0, field_4);

		xglDisableClientState(GL_VERTEX_ARRAY);
		if (m_bHaveColor)
			xglDisableClientState(GL_COLOR_ARRAY);
		if (m_bHaveTex)
			xglDisableClientState(GL_TEXTURE_COORD_ARRAY);
	}

	clear();
}

void Tesselator::endDrop()
{
	field_34 = false;
	clear();
}

RenderChunk Tesselator::end(int vboIdx)
{
	if (!field_34 || field_28)
		return RenderChunk(); // empty render chunk

	int count = field_4;

	field_34 = false;

	if (count > 0)
	{
		field_3C++;
		if (field_3C >= m_vboCount)
			field_3C = 0;
		if (vboIdx < 0)
			vboIdx = m_pVBOs[field_3C];

		xglBindBuffer(GL_ARRAY_BUFFER, vboIdx);
		xglBufferData(GL_ARRAY_BUFFER, sizeof (Vertex) * m_nVertices, m_pVertices, m_accessMode == 1 ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);

		field_48 += sizeof (Vertex) * m_nVertices;
	}

	clear();

	RenderChunk rchk(vboIdx, count);

	m_VboIdxToRenderChunkID[vboIdx] = rchk.m_id;

	return rchk;
}

int Tesselator::getVboCount()
{
	return m_vboCount;
}

void Tesselator::init()
{
	xglGenBuffers(m_vboCount, m_pVBOs);
}

void Tesselator::noColor()
{
	m_bBlockColor = true;
}

void Tesselator::normal(float x, float y, float z)
{
	// don't get the point of this
	dword_2514A4++;
}

void Tesselator::offset(float x, float y, float z)
{
	m_offsetX = x;
	m_offsetY = y;
	m_offsetZ = z;
}

void Tesselator::setAccessMode(int mode)
{
	m_accessMode = mode;
}

void Tesselator::tex(float u, float v)
{
	m_nextVtxU = u;
	m_nextVtxV = v;
	m_bHaveTex = true;
}

void Tesselator::vertexUV(float x, float y, float z, float u, float v)
{
	tex(u, v);
	vertex(x, y, z);
}

void Tesselator::vertex(float x, float y, float z)
{
	if (m_nVertices >= m_maxVertices) {
		LOG_W("Overwriting the vertex buffer! This chunk/entity won't show up");
		clear();
	}

	field_30++;
	if (m_drawArraysMode == GL_QUADS && (field_30 & 3) == 0)
	{
		for (int idx = 3; idx != 1; idx--)
		{
			// duplicate the last 2 added vertices in quad mode
			Vertex *pVert1 = &m_pVertices[m_nVertices - idx], *pVert2 = &m_pVertices[m_nVertices];
			if (m_bHaveTex)
			{
				pVert2->m_u = pVert1->m_u;
				pVert2->m_v = pVert1->m_v;
			}

			if (m_bHaveColor)
			{
				pVert2->m_color = pVert1->m_color;
			}

			pVert2->m_x = pVert1->m_x;
			pVert2->m_y = pVert1->m_y;
			pVert2->m_z = pVert1->m_z;

			field_4++;
			m_nVertices++;

#ifdef _DEBUG
			g_nVertices++;
#endif

			if (m_nVertices >= m_maxVertices)
				return;
		}
	}

	Vertex* pVert = &m_pVertices[m_nVertices];
	if (m_bHaveTex)
	{
		pVert->m_u = m_nextVtxU;
		pVert->m_v = m_nextVtxV;
	}

	if (m_bHaveColor)
	{
		pVert->m_color = m_nextVtxColor;
	}

	pVert->m_x = m_offsetX + x;
	pVert->m_y = m_offsetY + y;
	pVert->m_z = m_offsetZ + z;

	field_4++;
	m_nVertices++;
}

void Tesselator::voidBeginAndEndCalls(bool b)
{
	field_28 = b;
}

bool Tesselator::empty()
{
	return m_nVertices == 0;
}
