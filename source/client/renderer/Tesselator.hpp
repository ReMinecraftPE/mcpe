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
#include "RenderChunk.hpp"
#include "world/phys/Vec2.hpp"
#include "world/phys/Vec3.hpp"
#include "common/math/Color.hpp"
#include "renderer/VertexFormat.hpp"
#include "renderer/hal/enums/PrimitiveMode.hpp"
#include "renderer/Mesh.hpp"

#define TRIANGLE_MODE true
// false on Java
#define USE_VBO true

class Tesselator
{
public:
	static Tesselator instance; // singleton

public:
	class CurrentVertexPointers
	{
	public:
		Vec3* pos;
		uint32_t* color;
		uint32_t* normal;
		void* uvs[2];
		const mce::VertexFormat* pFormat;

	private:
		void _init();

	public:
		CurrentVertexPointers();
		CurrentVertexPointers(void* vertexData, const mce::VertexFormat& vertexFormat);

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
	void begin(int maxVertices);
	void begin(mce::PrimitiveMode mode, int maxVertices);
	void beginIndices(int maxIndices);
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
	void draw(const mce::MaterialPtr& materialPtr);
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

	mce::Mesh end(const char* debugName = nullptr, bool temporary = false);

	bool isFormatFixed() const;

private:
	CurrentVertexPointers m_currentVertex;

	std::vector<uint8_t> m_indices;
	bool m_bHasIndices;

	uint8_t m_indexSize;
	//_BYTE gap2D[3];
	unsigned int m_indexCount;
	mce::VertexFormat m_vertexFormat;

	// Tesselation state
	unsigned int m_vertices;

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

	// State
	bool m_bTesselating;
	mce::PrimitiveMode m_drawMode;

	// Buffer state
	int m_maxVertices;

private:
	bool m_bVoidBeginEnd;
};

