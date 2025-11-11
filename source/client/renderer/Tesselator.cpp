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
#include "renderer/hal/interface/RenderContext.hpp"

#include <cstddef>

int g_nVertices = 0, g_nTriangles = 0;

Tesselator Tesselator::instance;

void Tesselator::CurrentVertexPointers::_init()
{
	pos = nullptr;
	color = nullptr;
	normal = nullptr;
	uvs[0] = nullptr;
	uvs[1] = nullptr;
	pFormat = nullptr;
}

Tesselator::CurrentVertexPointers::CurrentVertexPointers()
{
	_init();
}

Tesselator::CurrentVertexPointers::CurrentVertexPointers(uint8_t* vertexData, const mce::VertexFormat& vertexFormat)
{
	_init();

	pFormat = &vertexFormat;

	pos = (Vec3*)vertexFormat.getFieldOffset(mce::VERTEX_FIELD_POSITION, vertexData);
	
	if (vertexFormat.hasField(mce::VERTEX_FIELD_COLOR))
	{
		color = (uint32_t*)vertexFormat.getFieldOffset(mce::VERTEX_FIELD_COLOR, vertexData);
	}

	if (vertexFormat.hasField(mce::VERTEX_FIELD_NORMAL))
	{
		normal = (uint32_t*)vertexFormat.getFieldOffset(mce::VERTEX_FIELD_NORMAL, vertexData);
	}

	if (vertexFormat.hasField(mce::VERTEX_FIELD_UV0))
	{
		uvs[0] = (Vec2*)vertexFormat.getFieldOffset(mce::VERTEX_FIELD_UV0, vertexData);
	}

	if (vertexFormat.hasField(mce::VERTEX_FIELD_UV1))
	{
		uvs[1] = (Vec2*)vertexFormat.getFieldOffset(mce::VERTEX_FIELD_UV1, vertexData);
	}
}

void Tesselator::CurrentVertexPointers::nextVertex()
{
	unsigned int vertexSize = pFormat->getVertexSize();

	pos += vertexSize / sizeof(Vec3);

	if (color) color += vertexSize / sizeof(uint32_t);
	if (normal) normal += vertexSize / sizeof(uint32_t);
	if (uvs[0]) uvs[0] += vertexSize / sizeof(Vec2);
	if (uvs[1]) uvs[1] += vertexSize / sizeof(Vec2);
}

void Tesselator::CurrentVertexPointers::clear()
{
	pos = nullptr;
	color = nullptr;
	normal = nullptr;
}

void Tesselator::_init()
{
	m_bHasIndices = false;

	m_indexSize = 0;
	m_indexCount = 0;

	m_vertices = 0;

	m_pendingVertices = 0;

	resetScale();

	m_nextVtxUVs[0] = Vec2::ZERO;
	m_nextVtxUVs[1] = Vec2::ZERO;
	m_nextVtxColor = 0;
	m_nextVtxNormal = 0;

	resetTilt();

	m_count = 0;
	m_bNoColorFlag = false;

	m_drawArraysMode = 0;

	m_bTesselating = false;
	m_mode = mce::PRIMITIVE_MODE_NONE;

	m_vboCounts = 1024; // 10 on Java
	m_vboId = -1;

	m_bVoidBeginEnd = false;

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

void* Tesselator::_allocateIndices(int count)
{
	m_indices.resize(m_indexSize * count);
	return &m_indices.front();
}

void Tesselator::_tex(const Vec2& uv, int count)
{
	m_nextVtxUVs[count] = uv;

	if (!isFormatFixed())
	{
		m_vertexFormat.enableField((mce::VertexField)(mce::VERTEX_FIELD_UV0 + count));
	}
}

void Tesselator::clear()
{
	// @HAL remove
	m_accessMode = 2;
	
	m_count = 0;
	m_indices.clear();
	m_vertexFormat = mce::VertexFormat::EMPTY;
	m_currentVertex.clear();
	m_indexCount = 0;
	m_vertices = 0;
	m_bVoidBeginEnd = false;
	m_bTesselating = false;
	m_bHasIndices = false;
}

void Tesselator::cancel()
{
	m_bTesselating = false;
}

void Tesselator::color(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
#if MC_ENDIANNESS_BIG
	colorABGR(a | (b << 8) | (g << 16) | (r << 24));
#else // MC_ENDIANNESS_LITTLE
	colorABGR((a << 24) | (b << 16) | (g << 8) | r);
#endif
}

void Tesselator::color(uint8_t r, uint8_t g, uint8_t b)
{
	color(r, g, b, (uint8_t)255);
}

void Tesselator::color(const Color& c)
{
	color(int8_t(c.r * 255), int8_t(c.g * 255), int8_t(c.b * 255), int8_t(c.a * 255));
}

void Tesselator::color(int32_t c, int a)
{
	color((c >> 16) & 0xFF, (c >> 8) & 0xFF, c & 0xFF, a);
}

void Tesselator::color(int32_t c)
{
	color((c >> 16) & 0xFF, (c >> 8) & 0xFF, c & 0xFF, 255);
}

void Tesselator::color(int r, int g, int b, int a)
{
	color(uint8_t(r), uint8_t(g), uint8_t(b), uint8_t(a));
}

void Tesselator::color(float r, float g, float b)
{
	color(r, g, b, 1.0f);
}

void Tesselator::color(float r, float g, float b, float a)
{
	color(Color(r, g, b, a));
}

void Tesselator::colorABGR(uint32_t c)
{
	if (m_bNoColorFlag) return;

	m_nextVtxColor = c;

	if (!isFormatFixed())
	{
		m_vertexFormat.enableField(mce::VERTEX_FIELD_COLOR);
	}
}

void Tesselator::begin(int maxVertices)
{
	begin(mce::PRIMITIVE_MODE_QUAD_LIST, maxVertices);
}

void Tesselator::begin(mce::PrimitiveMode mode, int maxVertices)
{
	if (m_bTesselating || m_bVoidBeginEnd) return;

	clear();

	m_mode = mode;
	m_bNoColorFlag = false;
	m_bTesselating = true;
	m_vertexFormat.enableField(mce::VERTEX_FIELD_POSITION);
	m_indexSize = 0;
	m_indexCount = 0;
	m_pendingVertices = maxVertices;

	// @HAL: remove
	m_drawArraysMode = mce::modeMap[mode];
	if (mode == mce::PRIMITIVE_MODE_QUAD_LIST)
	{
		m_drawArraysMode = GL_QUADS;
	}
}

void Tesselator::draw(const mce::MaterialPtr& materialPtr)
{
	if (!m_bTesselating || m_bVoidBeginEnd)
		return;

	m_bTesselating = false;

	if (m_vertices > 0)
	{
		if (materialPtr != mce::MaterialPtr::NONE)
		{
			mce::Mesh mesh = end("draw", true);
			mesh.render(materialPtr);
		}
		else
		{
			if (m_bVboMode)
			{
				/*m_vboId++;
				if (m_vboId >= m_vboCounts)
					m_vboId = 0;*/
				m_vboId = (m_vboId + 1) % m_vboCounts;

				xglBindBuffer(GL_ARRAY_BUFFER, m_vboIds[m_vboId]);
				xglBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * m_vertices, m_pVertices, m_accessMode == 1 ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
			}

			if (m_vertexFormat.hasField(mce::VERTEX_FIELD_UV0))
			{
				// it won't use address 12, because we've bound a buffer object
				xglTexCoordPointer(2, GL_FLOAT, sizeof(Vertex), (void*)offsetof(Vertex, m_u));
				xglEnableClientState(GL_TEXTURE_COORD_ARRAY);
			}
			if (m_vertexFormat.hasField(mce::VERTEX_FIELD_COLOR))
			{
				// it won't use address 12, because we've bound a buffer object
				xglColorPointer(4, GL_UNSIGNED_BYTE, sizeof(Vertex), (void*)offsetof(Vertex, m_color));
				xglEnableClientState(GL_COLOR_ARRAY);
			}
#ifdef USE_GL_NORMAL_LIGHTING
			if (m_vertexFormat.hasField(mce::VERTEX_FIELD_NORMAL))
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
			if (m_vertexFormat.hasField(mce::VERTEX_FIELD_NORMAL))
				xglDisableClientState(GL_NORMAL_ARRAY);
#endif
			if (m_vertexFormat.hasField(mce::VERTEX_FIELD_COLOR))
				xglDisableClientState(GL_COLOR_ARRAY);
			if (m_vertexFormat.hasField(mce::VERTEX_FIELD_UV0))
				xglDisableClientState(GL_TEXTURE_COORD_ARRAY);
		}
	}

	clear();
}

mce::Mesh Tesselator::end(const char* debugName, bool temporary)
{
	if (!m_bTesselating || m_bVoidBeginEnd)
	{
		return mce::Mesh();
	}

	if (m_vertices && isFormatFixed())
	{
		mce::Mesh mesh(m_vertexFormat,
			m_vertices,
			m_indexCount,
			m_indexSize,
			m_mode,
			&m_indices.front(),
			temporary);

		if (!temporary)
			clear();

		return mesh;
	}
	else
	{
		m_bTesselating = false;
		return mce::Mesh();
	}
}

RenderChunk Tesselator::end(int vboIdx)
{
	if (!m_bTesselating || m_bVoidBeginEnd)
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
			xglBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * m_vertices, m_pVertices, m_accessMode == 1 ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
		}

		field_48 += sizeof (Vertex) * m_vertices;
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

void Tesselator::trim()
{
	m_indices.clear();
}

void Tesselator::enableColor()
{
	m_bNoColorFlag = false;
}

void Tesselator::noColor()
{
	m_bNoColorFlag = true;
}

void Tesselator::tilt()
{
	m_bTilt = true;
}

void Tesselator::resetTilt()
{
	m_bTilt = false;
}

void Tesselator::scale2d(float x, float y)
{
	m_scale2D.x *= x;
	m_scale2D.y *= y;
}

void Tesselator::scale3d(float x, float y, float z)
{
	m_scale3D.x *= x;
	m_scale3D.y *= y;
	m_scale3D.z *= z;
}

void Tesselator::resetScale()
{
	m_scale2D = Vec2::ONE;
	m_scale3D = Vec3::ONE;
}

void Tesselator::normal(float x, float y, float z)
{
#ifdef USE_GL_NORMAL_LIGHTING
	/*if (!m_bTesselating)
		LOG_W("But...");*/

	int8_t bx = static_cast<int8_t>(ceilf(x * 127));
	int8_t by = static_cast<int8_t>(ceilf(y * 127));
	int8_t bz = static_cast<int8_t>(ceilf(z * 127));
#if MC_ENDIANNESS_BIG
	m_nextVtxNormal = (bx << 24) | (by << 16) | (bz << 8);
#else // MC_ENDIANNESS_LITTLE
	m_nextVtxNormal = (bx << 0) | (by << 8) | (bz << 16);
#endif

	if (!isFormatFixed())
	{
		m_vertexFormat.enableField(mce::VERTEX_FIELD_NORMAL);
	}
#endif
}

void Tesselator::setOffset(const Vec3& pos)
{
	m_offset = pos;
}

void Tesselator::addOffset(const Vec3& pos)
{
	m_offset += pos;
}

void Tesselator::setAccessMode(int mode)
{
	m_accessMode = mode;
}

void Tesselator::tex(const Vec2& uv)
{
	_tex(uv, 0);
}

void Tesselator::tex1(const Vec2& uv)
{
	_tex(uv, 1);
}

void Tesselator::tex(float u, float v)
{
	tex(Vec2(u, v));
}

void Tesselator::tex1(float u, float v)
{
	tex1(Vec2(u, v));
}

void Tesselator::triangle(unsigned int x, unsigned int y, unsigned int z)
{
	uint8_t* indices = (uint8_t*)_allocateIndices(3);
	indices[0 * m_indexSize] = x;
	indices[1 * m_indexSize] = y;
	indices[2 * m_indexSize] = z;
	m_indexCount += 3;
}

void Tesselator::vertex(float x, float y, float z)
{
	if (m_vertices >= m_maxVertices) {
		LOG_W("Overwriting the vertex buffer! This chunk/entity won't show up");
		clear();
	}

	if (m_count == mce::RenderContext::getMaxVertexCount())
		return;

	m_count++;

	uint8_t* oldIndicesPtr = !m_indices.empty() ? &m_indices.front() : nullptr;
	unsigned int vertexSize = m_vertexFormat.getVertexSize();

	if (m_pendingVertices > 0)
	{
		m_indices.reserve(vertexSize * m_pendingVertices);
		m_pendingVertices = 0;
	}

	m_indices.resize((m_vertices+1) * vertexSize);

	// Make sure m_indices front pointer hasn't changed from reallocation as a result of reserve or resize
	if (isFormatFixed() && oldIndicesPtr == &m_indices.front())
	{
		m_currentVertex.nextVertex();
	}
	else
	{
		m_currentVertex = CurrentVertexPointers(&m_indices[m_vertices * vertexSize], m_vertexFormat);
	}

	if (m_bTilt)
	{
		LOG_E("Tilt support not implemented");
		throw std::bad_cast();
	}

	// Set vertex pos
	m_currentVertex.pos->x = (m_offset.x + (x * m_scale3D.x)) * m_scale2D.x;
	m_currentVertex.pos->y = (m_offset.y + (y * m_scale3D.x)) * m_scale2D.y;
	m_currentVertex.pos->z =  m_offset.z + (z * m_scale3D.x);

	// Set vertex UVs
	for (int i = 0; i < 2; i++)
	{
		if (m_currentVertex.uvs[i])
		{
			m_currentVertex.uvs[i]->x = ceilf(m_nextVtxUVs[i].x * 65535.f);
			m_currentVertex.uvs[i]->x = ceilf(m_nextVtxUVs[i].y * 65535.f);
		}
	}

	// Set vertex color
	if (m_currentVertex.color)
		*m_currentVertex.color = m_nextVtxColor;

	// Set vertex normal
	if (m_currentVertex.normal)
		*m_currentVertex.normal = m_nextVtxNormal;

	if (m_drawArraysMode == GL_QUADS && TRIANGLE_MODE && (m_count % 4) == 0)
	{
		for (int idx = 3; idx != 1; idx--)
		{
			// duplicate the last 2 added vertices in quad mode
			Vertex *pVert1 = &m_pVertices[m_vertices - idx], *pVert2 = &m_pVertices[m_vertices];
			if (m_vertexFormat.hasField(mce::VERTEX_FIELD_UV0))
			{
				pVert2->m_u = pVert1->m_u;
				pVert2->m_v = pVert1->m_v;
			}

			if (m_vertexFormat.hasField(mce::VERTEX_FIELD_COLOR))
			{
				pVert2->m_color = pVert1->m_color;
			}
            
            // Wasn't here in Java cuz I guess it's not needed?
			// TBR: This is needed with Emscripten's OpenGL implementation.
#ifdef USE_GL_NORMAL_LIGHTING
            if (m_vertexFormat.hasField(mce::VERTEX_FIELD_NORMAL))
            {
                pVert2->m_normal = pVert1->m_normal;
            }
#endif

			pVert2->m_x = pVert1->m_x;
			pVert2->m_y = pVert1->m_y;
			pVert2->m_z = pVert1->m_z;

			m_vertices++;

#ifdef _DEBUG
			g_nVertices++;
#endif

			if (m_vertices >= m_maxVertices)
				return;
		}
	}

	Vertex* pVert = &m_pVertices[m_vertices];
	if (m_vertexFormat.hasField(mce::VERTEX_FIELD_UV0))
	{
		pVert->m_u = m_nextVtxUVs[0].x;
		pVert->m_v = m_nextVtxUVs[0].y;
	}

	if (m_vertexFormat.hasField(mce::VERTEX_FIELD_COLOR))
	{
		pVert->m_color = m_nextVtxColor;
	}

#ifdef USE_GL_NORMAL_LIGHTING
	if (m_vertexFormat.hasField(mce::VERTEX_FIELD_NORMAL))
	{
		pVert->m_normal = m_nextVtxNormal;
	}
#endif

	pVert->m_x = m_offset.x + x;
	pVert->m_y = m_offset.y + y;
	pVert->m_z = m_offset.z + z;

	m_vertices++;
}

void Tesselator::vertexUV(const Vec3& pos, float u, float v)
{
	tex(u, v);
	vertex(pos);
}

void Tesselator::voidBeginAndEndCalls(bool b)
{
	m_bVoidBeginEnd = b;
}
