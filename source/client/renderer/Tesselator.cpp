/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include <cstddef>
#include "Tesselator.hpp"
#include "thirdparty/GL/GL.hpp"
#include "GameMods.hpp"
#include "common/Logger.hpp"
#include "compat/EndianDefinitions.h"
#include "renderer/RenderContextImmediate.hpp"

int g_nVertices = 0, g_nTriangles = 0;

Tesselator Tesselator::instance;

void Tesselator::CurrentVertexPointers::_init()
{
	clear();
	uvs[0] = nullptr;
	uvs[1] = nullptr;
	pFormat = nullptr;
}

Tesselator::CurrentVertexPointers::CurrentVertexPointers()
{
	_init();
}

Tesselator::CurrentVertexPointers::CurrentVertexPointers(void* vertexData, const mce::VertexFormat& vertexFormat)
{
	_init();

	pFormat = &vertexFormat;

	pos = (Vec3*)vertexFormat.getFieldOffset(mce::VERTEX_FIELD_POSITION, vertexData);
	
	if (vertexFormat.hasField(mce::VERTEX_FIELD_COLOR))
	{
		color = (uint32_t*)vertexFormat.getFieldOffset(mce::VERTEX_FIELD_COLOR, vertexData);
	}

#ifdef USE_GL_NORMAL_LIGHTING
	if (vertexFormat.hasField(mce::VERTEX_FIELD_NORMAL))
	{
		normal = (uint32_t*)vertexFormat.getFieldOffset(mce::VERTEX_FIELD_NORMAL, vertexData);
	}
#endif

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

	pos = (Vec3*)((uint8_t*)pos + vertexSize);

	if (color)  color  = (uint32_t*)((uint8_t*)color + vertexSize);
	if (normal) normal = (uint32_t*)((uint8_t*)normal + vertexSize);
	if (uvs[0]) uvs[0] = (Vec2*)((uint8_t*)uvs[0] + vertexSize);
	if (uvs[1]) uvs[1] = (Vec2*)((uint8_t*)uvs[1] + vertexSize);
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

	m_nextVtxColor = 0;
	m_nextVtxNormal = 0;

	resetTilt();

	m_count = 0;
	m_bNoColorFlag = false;

	m_bTesselating = false;
	m_drawMode = mce::PRIMITIVE_MODE_NONE;

	m_vboCounts = 1024; // 10 on Java
	m_vboId = -1;

	m_bVoidBeginEnd = false;

	m_accessMode = 2;
}

Tesselator::Tesselator(int size)
{
	_init();

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

	m_drawMode = mode;
	m_bNoColorFlag = false;
	m_bTesselating = true;
	m_vertexFormat.enableField(mce::VERTEX_FIELD_POSITION);
	m_indexSize = 0;
	m_indexCount = 0;
	m_pendingVertices = maxVertices;
}

void Tesselator::draw(const mce::MaterialPtr& materialPtr)
{
	if (!m_bTesselating || m_bVoidBeginEnd)
		return;

	if (m_vertices > 0)
	{
		mce::Mesh mesh = end("draw", true);
		mesh.render(materialPtr);
	}

	clear();
}

mce::Mesh Tesselator::end(const char* debugName, bool temporary)
{
	if (!m_bTesselating || m_bVoidBeginEnd)
	{
		//LOG_W("Not tesselating!");
		return mce::Mesh();
	}

	if (m_vertices && isFormatFixed())
	{
		mce::Mesh mesh(m_vertexFormat,
			m_vertices,
			m_indexCount,
			m_indexSize,
			m_drawMode,
			&m_indices[0],
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

bool Tesselator::isFormatFixed() const
{
	return m_currentVertex.pos != nullptr;
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
	// We cannot use Vec2::ONE here since Tesselator::instance is initialized dynamically before Vec2::ONE
	m_scale2D = Vec2(1, 1);
	m_scale3D = Vec3(1, 1, 1);
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
	if (m_count == mce::RenderContext::getMaxVertexCount())
		return;

	m_count++;

	unsigned int vertexSize = m_vertexFormat.getVertexSize();
	uint8_t* oldIndicesPtr = !m_indices.empty() ? &m_indices.front() : nullptr;

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
			m_currentVertex.uvs[i]->x = m_nextVtxUVs[0].x;//ceilf(m_nextVtxUVs[i].x * 65535.f);
			m_currentVertex.uvs[i]->y = m_nextVtxUVs[0].y;//ceilf(m_nextVtxUVs[i].y * 65535.f);
		}
	}

	// Set vertex color
	if (m_currentVertex.color)
		*m_currentVertex.color = m_nextVtxColor;

	// Set vertex normal
	if (m_currentVertex.normal)
		*m_currentVertex.normal = m_nextVtxNormal;

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
