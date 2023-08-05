/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "compat/GL.hpp"
#include "Tesselator.hpp"

#include <cstddef>

int dword_2514A4 = 0;

Tesselator Tesselator::instance;

Tesselator::Tesselator(int allotedSize)
{
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
	field_2C = 0;
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
		xglBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * field_2C, m_pVertices, m_accessMode == 1 ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);

		if (m_bHaveTex)
		{
			// it won't use address 12, because we've bound a buffer object
			glTexCoordPointer(2, GL_FLOAT, sizeof(Vertex), (void*)offsetof(Vertex, m_u));
			glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		}
		if (m_bHaveColor)
		{
			// it won't use address 12, because we've bound a buffer object
			glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(Vertex), (void*)offsetof(Vertex, m_color));
			glEnableClientState(GL_COLOR_ARRAY);
		}

		glVertexPointer(3, GL_FLOAT, sizeof(Vertex), (void*)offsetof(Vertex, m_x));
		glEnableClientState(GL_VERTEX_ARRAY);

		// if we want to draw quads, draw triangles actually
		// otherwise, just pass the mode, it's fine
		if (m_drawArraysMode == GL_QUADS)
			glDrawArrays(GL_TRIANGLES, 0, field_4);
		else
			glDrawArrays(m_drawArraysMode, 0, field_4);

		glDisableClientState(GL_VERTEX_ARRAY);
		if (m_bHaveColor)
			glDisableClientState(GL_COLOR_ARRAY);
		if (m_bHaveTex)
			glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	}

	clear();
}

RenderChunk Tesselator::end(int vboIdx)
{
	if (!field_34 || field_28)
		return RenderChunk(); // empty render chunk

	int count = field_4;

	field_34 = 0;

	if (count > 0)
	{
		field_3C++;
		if (field_3C >= m_vboCount)
			field_3C = 0;
		if (vboIdx < 0)
			vboIdx = m_pVBOs[field_3C];

		xglBindBuffer(GL_ARRAY_BUFFER, vboIdx);
		xglBufferData(GL_ARRAY_BUFFER, sizeof (Vertex) * field_2C, m_pVertices, m_accessMode == 1 ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);

		field_48 += sizeof (Vertex) * field_2C;
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
	field_30++;
	if (m_drawArraysMode == GL_QUADS && !(field_30 << 30))
	{
		for (int v18 = 3; v18 != 1; v18--)
		{
			// why the hell is it doing this
			Vertex *pVert1 = &m_pVertices[field_2C - v18], *pVert2 = &m_pVertices[field_2C];
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
			field_2C++;
		}
	}

	Vertex* pVert = &m_pVertices[field_2C];
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
	field_2C++;

	if (!(field_4 & 3) && field_2C >= m_maxVertices - 1)
	{
		for (int i = 0; i < 3; i++)
			puts("Overwriting the vertex buffer! This chunk/entity won't show up");

		clear();
	}
}

void Tesselator::voidBeginAndEndCalls(bool b)
{
	field_28 = b;
}
