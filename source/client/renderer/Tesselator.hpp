/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include <stdint.h>
#include <map>
#include "thirdparty/GL/GL.hpp"
#include "RenderChunk.hpp"
#include "world/phys/Vec3.hpp"

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

private:
	void _init();

public:
	Tesselator(int size = 0x800000);
	~Tesselator();

	void addOffset(float x, float y, float z);
	void  addOffset(const Vec3& pos) { addOffset(pos.x, pos.y, pos.z); }
	void begin();
	void begin(GLenum mode);
	void clear();
	void color(int c);
	void color(int c, int a);
	void color(int r, int g, int b);
	void color(int r, int g, int b, int a);
	void color(char r, char g, char b);
	void color(float r, float g, float b);
	void color(float r, float g, float b, float a);
	void draw();
	int  getVboCount();
	void init();
	void noColor();
	void normal(float, float, float);
	void normal(const Vec3& pos) { normal(pos.x, pos.y, pos.z); }
	void offset(float, float, float);
	void offset(const Vec3& pos) { offset(pos.x, pos.y, pos.z); }
	void setAccessMode(int mode); // sets m_DrawArraysMode
	void tex(float u, float v);
	void vertex(float x, float y, float z);
	void vertex(const Vec3& pos) { vertex(pos.x, pos.y, pos.z); }
	void vertexUV(float x, float y, float z, float u, float v);
	void vertexUV(const Vec3& pos, float u, float v) { vertexUV(pos.x, pos.y, pos.z, u, v); }
	void voidBeginAndEndCalls(bool b);

	RenderChunk end(int);

private:
	// Buffer
	Vertex* m_pVertices;

	// Tesselation state
	int m_vertices;

	float m_nextVtxU; // u
	float m_nextVtxV; // v
	uint32_t m_nextVtxColor; // col

	bool m_bHasColor;
	bool m_bHasTexture;
	bool m_bHasNormal;

	int m_count;
	bool m_bNoColorFlag;

	GLenum m_drawArraysMode; // draw_mode

	float m_offsetX;
	float m_offsetY;
	float m_offsetZ;

	uint32_t m_nextVtxNormal; // normalValue

	// State
	bool m_bTesselating;
	bool m_bVboMode;

	// VBO State
	GLuint* m_vboIds;
	int m_vboId;
	int m_vboCounts;

	// Buffer state
	int m_maxVertices;

private:
	bool field_28;
	int m_nVertices;

	int field_48;

	int m_accessMode;

	std::map<uint32_t, uint32_t> m_VboIdxToRenderChunkID;
};

