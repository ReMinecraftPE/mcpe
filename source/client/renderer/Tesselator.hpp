/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include <stdint.h>
#include <map>
#include <vector>
#include "thirdparty/GL/GL.hpp"
#include "RenderChunk.hpp"
#include "world/phys/Vec2.hpp"
#include "world/phys/Vec3.hpp"
#include "common/math/Color.hpp"
#include "renderer/VertexFormat.hpp"
#include "renderer/hal/enums/PrimitiveMode.hpp"

#define GET_RED(c)   (uint8_t(((c) >>  0) & 0xFF))
#define GET_GREEN(c) (uint8_t(((c) >>  8) & 0xFF))
#define GET_BLUE(c)  (uint8_t(((c) >> 16) & 0xFF))
#define GET_ALPHA(c) (uint8_t(((c) >> 24) & 0xFF))

#define TRIANGLE_MODE true
// false on Java
#define USE_VBO true

class Tesselator
{
public:
	static Tesselator instance; // singleton

public:
	struct Vertex
	{
		// position
		float m_x;
		float m_y;
		float m_z;
		// texture mapping coords
		float m_u;
		float m_v;
		// RGBA color
		uint32_t m_color;
#ifdef USE_GL_NORMAL_LIGHTING
		// the legend
        uint32_t m_normal;
#endif
	};

	class CurrentVertexPointers
	{
	public:
		Vec3* pos;
		uint32_t* color;
		uint32_t* normal;
		Vec2* uvs[2];
		const mce::VertexFormat* pFormat;

	private:
		void _init();

	public:
		CurrentVertexPointers();
		CurrentVertexPointers(uint8_t* vertexData, const mce::VertexFormat& vertexFormat);

	public:
		void nextVertex();
		void clear();
	};

private:
	void _init();

public:
	Tesselator(int size = 0x800000);
	~Tesselator();

private:
	void* _allocateIndices(int count);
	void _tex(const Vec2& uv, int count);

public:
	void begin(int maxVertices = 0);
	void begin(mce::PrimitiveMode mode, int maxVertices = 0);
	void clear();
	void cancel();
	void color(int32_t c);
	void color(int32_t c, int a);
	void color(uint8_t r, uint8_t g, uint8_t b);
	void color(uint8_t r, uint8_t g, uint8_t b, uint8_t a);
	void color(const Color& c);
	void color(int r, int g, int b, int a);
	void color(float r, float g, float b);
	void color(float r, float g, float b, float a);
	void colorABGR(uint32_t c);
	void draw();
	int  getVboCount();
	void init();
	void trim();
	void enableColor();
	void noColor();
	void tilt();
	void resetTilt();
	void scale2d(float x, float y);
	void scale3d(float x, float y, float z);
	void resetScale();
	void normal(float x, float y, float z);
	void normal(const Vec3& pos) { normal(pos.x, pos.y, pos.z); }
	void setOffset(const Vec3& pos);
	void addOffset(const Vec3& pos);
	void setOffset(float x, float y, float z) { setOffset(Vec3(x, y, z)); }
	void addOffset(float x, float y, float z) { addOffset(Vec3(x, y, z)); }
	void setAccessMode(int mode); // sets m_DrawArraysMode
	void tex(const Vec2& uv);
	void tex1(const Vec2& uv);
	void tex(float u, float v);
	void tex1(float u, float v);
	void triangle(unsigned int x, unsigned int y, unsigned int z);
	void vertex(float x, float y, float z);
	void vertex(const Vec3& pos) { vertex(pos.x, pos.y, pos.z); }
	void vertexUV(const Vec3& pos, float u, float v);
	void vertexUV(float x, float y, float z, float u, float v) { vertexUV(Vec3(x, y, z), u, v); }
	void voidBeginAndEndCalls(bool b);

	RenderChunk end(int);

	bool isFormatFixed() const { return m_currentVertex.pos != nullptr; }

private:
	CurrentVertexPointers m_currentVertex;

	// Buffer
	Vertex* m_pVertices;

	std::vector<uint8_t> m_indices;
	bool m_bHasIndices;

	uint8_t m_indexSize;
	//_BYTE gap2D[3];
	unsigned int m_indexCount;
	mce::VertexFormat m_vertexFormat;

	// @HAL: remove
	// Tesselation state
	int m_vertices;

	int m_pendingVertices;

	Vec3 m_offset;
	Vec3 m_scale3D;
	Vec2 m_scale2D;

	Vec2 m_nextVtxUVs[2]; // u, v
	uint32_t m_nextVtxColor; // col
	uint32_t m_nextVtxNormal; // normalValue

	bool m_bTilt;

	int m_count;
	bool m_bNoColorFlag;

	// @HAL: remove
	GLenum m_drawArraysMode; // draw_mode

	// State
	bool m_bTesselating;
	mce::PrimitiveMode m_mode;
	bool m_bVboMode;

	// VBO State
	GLuint* m_vboIds;
	int m_vboId;
	int m_vboCounts;

	// Buffer state
	int m_maxVertices;

private:
	bool m_bVoidBeginEnd;
	unsigned int m_nVertices;

	int field_48;

	int m_accessMode;

	std::map<uint32_t, uint32_t> m_VboIdxToRenderChunkID;
};

