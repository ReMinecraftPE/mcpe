/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "Tesselator.hpp"
#include "thirdparty/GL/GL.hpp"
#include "common/Logger.hpp"
#include "compat/EndianDefinitions.h"

#include <cstddef>

int g_nVertices = 0, g_nTriangles = 0;

Tesselator Tesselator::instance;

void Tesselator::_init()
{
	m_vertices = 0;

	m_nextVtxU = 0;
	m_nextVtxV = 0;
	m_nextVtxColor = 0;

	m_bHasColor = false;
	m_bHasTexture = false;
	m_bHasNormal = false;

	m_count = 0;
	m_bNoColorFlag = false;

	m_drawArraysMode = 0;

	m_offsetX = 0.0f;
	m_offsetY = 0.0f;
	m_offsetZ = 0.0f;

	m_nextVtxNormal = 0;

	m_bTesselating = false;

	m_vboCounts = 1024; // 10 on Java
	m_vboId = -1;

	field_28 = false;
	m_nVertices = 0;

	field_48 = 0;

	m_accessMode = 2;
}

Tesselator::Tesselator(int size)
{
	_init();

	// Initialize buffer
	m_maxVertices = size / sizeof(Vertex);
	m_pVertices = new Vertex[m_maxVertices];

	// Setup VBO
	m_bVboMode = USE_VBO; // && lwjgl::GLContext::getCapabilities()["GL_ARB_vertex_buffer_object"];
	if (m_bVboMode)
	{
		m_vboIds = new GLuint[m_vboCounts];
		//init(); // Java
	}
}

Tesselator::~Tesselator()
{
	if (m_vboIds)
		delete[] m_vboIds;
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
	m_vertices = 0;
	m_count = 0;
	m_nVertices = 0;
	field_28 = 0;
}

void Tesselator::color(int r, int g, int b, int a)
{
	if (m_bNoColorFlag) return;

	if (b >= 255) b = 255;
	if (g >= 255) g = 255;
	if (r >= 255) r = 255;
	if (a >= 255) a = 255;

	// note: weird bit hacking (as in X & ~(X >> 31)) is used to perform the following:
	if (b < 0) b = 0;
	if (g < 0) g = 0;
	if (r < 0) r = 0;
	if (a < 0) a = 0;

	m_bHasColor = true;
#if MC_ENDIANNESS_BIG
	m_nextVtxColor = a | (b << 8) | (g << 16) | (r << 24);
#else // MC_ENDIANNESS_LITTLE
	m_nextVtxColor = (a << 24) | (b << 16) | (g << 8) | r;
#endif
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

void Tesselator::begin(GLenum mode)
{
	if (m_bTesselating || field_28) return;

	m_bTesselating = true;
	clear();
	m_drawArraysMode = mode;
	m_bHasNormal = false;
	m_bHasColor  = false;
	m_bHasTexture = false;
	m_bNoColorFlag = false;
}

void Tesselator::draw()
{
	if (!m_bTesselating || field_28)
		return;

	m_bTesselating = false;

	if (m_vertices > 0)
	{
		if (m_bVboMode)
		{
			/*m_vboId++;
			if (m_vboId >= m_vboCounts)
				m_vboId = 0;*/
			m_vboId = (m_vboId + 1) % m_vboCounts;

			xglBindBuffer(GL_ARRAY_BUFFER, m_vboIds[m_vboId]);
			xglBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * m_nVertices, m_pVertices, m_accessMode == 1 ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
		}

		if (m_bHasTexture)
		{
			// it won't use address 12, because we've bound a buffer object
			xglTexCoordPointer(2, GL_FLOAT, sizeof(Vertex), (void*)offsetof(Vertex, m_u));
			xglEnableClientState(GL_TEXTURE_COORD_ARRAY);
		}
		if (m_bHasColor)
		{
			// it won't use address 12, because we've bound a buffer object
			xglColorPointer(4, GL_UNSIGNED_BYTE, sizeof(Vertex), (void*)offsetof(Vertex, m_color));
			xglEnableClientState(GL_COLOR_ARRAY);
		}
#ifdef USE_GL_NORMAL_LIGHTING
		if (m_bHasNormal)
		{
			xglNormalPointer(GL_BYTE, sizeof(Vertex), (void*)offsetof(Vertex, m_normal));
			xglEnableClientState(GL_NORMAL_ARRAY);
		}
#endif

		xglVertexPointer(3, GL_FLOAT, sizeof(Vertex), (void*)offsetof(Vertex, m_x));
		xglEnableClientState(GL_VERTEX_ARRAY);

		// if we want to draw quads, draw triangles actually
		// otherwise, just pass the mode, it's fine
		if (m_drawArraysMode == GL_QUADS && TRIANGLE_MODE)
			xglDrawArrays(GL_TRIANGLES, 0, m_vertices);
		else
			xglDrawArrays(m_drawArraysMode, 0, m_vertices);

		xglDisableClientState(GL_VERTEX_ARRAY);
#ifdef USE_GL_NORMAL_LIGHTING
		if (m_bHasNormal)
			xglDisableClientState(GL_NORMAL_ARRAY);
#endif
		if (m_bHasColor)
			xglDisableClientState(GL_COLOR_ARRAY);
		if (m_bHasTexture)
			xglDisableClientState(GL_TEXTURE_COORD_ARRAY);
	}

	clear();
}

RenderChunk Tesselator::end(int vboIdx)
{
	if (!m_bTesselating || field_28)
	{
		//LOG_W("Not tesselating!");
		return RenderChunk(); // empty render chunk
	}
	int count = m_vertices;
	m_bTesselating = false;

	if (count > 0)
	{
		// Bind VBO
		if (m_bVboMode)
		{
			m_vboId++;
			if (m_vboId >= m_vboCounts)
				m_vboId = 0;
			//m_vboId = (m_vboId + 1) % m_vboCounts;
			if (vboIdx < 0)
				vboIdx = m_vboIds[m_vboId];

			xglBindBuffer(GL_ARRAY_BUFFER, vboIdx);
			xglBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * m_nVertices, m_pVertices, m_accessMode == 1 ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
		}

		field_48 += sizeof (Vertex) * m_nVertices;
	}

	clear();

	RenderChunk rchk(vboIdx, count);

	m_VboIdxToRenderChunkID[vboIdx] = rchk.m_id;

	return rchk;
}

int Tesselator::getVboCount()
{
	return m_vboCounts;
}

void Tesselator::init()
{
	xglGenBuffers(m_vboCounts, m_vboIds);
}

void Tesselator::noColor()
{
	m_bNoColorFlag = true;
}

void Tesselator::normal(float x, float y, float z)
{
#ifdef USE_GL_NORMAL_LIGHTING
	/*if (!m_bTesselating)
		LOG_W("But...");*/

	m_bHasNormal = true;
	int8_t bx = static_cast<int8_t>(x * 128);
	int8_t by = static_cast<int8_t>(y * 127);
	int8_t bz = static_cast<int8_t>(z * 127);
#if MC_ENDIANNESS_BIG
	m_nextVtxNormal = (bx << 24) | (by << 16) | (bz << 8);
#else // MC_ENDIANNESS_LITTLE
	m_nextVtxNormal = (bx << 0) | (by << 8) | (bz << 16);
#endif
#endif
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
	m_bHasTexture = true;
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

	m_count++;
	if (m_drawArraysMode == GL_QUADS && TRIANGLE_MODE && (m_count % 4) == 0)
	{
		for (int idx = 3; idx != 1; idx--)
		{
			// duplicate the last 2 added vertices in quad mode
			Vertex *pVert1 = &m_pVertices[m_nVertices - idx], *pVert2 = &m_pVertices[m_nVertices];
			if (m_bHasTexture)
			{
				pVert2->m_u = pVert1->m_u;
				pVert2->m_v = pVert1->m_v;
			}

			if (m_bHasColor)
			{
				pVert2->m_color = pVert1->m_color;
			}
            
            // Wasn't here in Java cuz I guess it's not needed?
			// TBR: This is needed with Emscripten's OpenGL implementation.
#ifdef USE_GL_NORMAL_LIGHTING
            if (m_bHasNormal)
            {
                pVert2->m_normal = pVert1->m_normal;
            }
#endif

			pVert2->m_x = pVert1->m_x;
			pVert2->m_y = pVert1->m_y;
			pVert2->m_z = pVert1->m_z;

			m_vertices++;
			m_nVertices++;

#ifdef _DEBUG
			g_nVertices++;
#endif

			if (m_nVertices >= m_maxVertices)
				return;
		}
	}

	Vertex* pVert = &m_pVertices[m_nVertices];
	if (m_bHasTexture)
	{
		pVert->m_u = m_nextVtxU;
		pVert->m_v = m_nextVtxV;
	}

	if (m_bHasColor)
	{
		pVert->m_color = m_nextVtxColor;
	}

#ifdef USE_GL_NORMAL_LIGHTING
	if (m_bHasNormal)
	{
		pVert->m_normal = m_nextVtxNormal;
	}
#endif

	pVert->m_x = m_offsetX + x;
	pVert->m_y = m_offsetY + y;
	pVert->m_z = m_offsetZ + z;

	m_vertices++;
	m_nVertices++;
}

void Tesselator::voidBeginAndEndCalls(bool b)
{
	field_28 = b;
}
