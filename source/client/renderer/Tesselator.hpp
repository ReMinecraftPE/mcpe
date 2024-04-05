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

#define GET_RED(c)   (uint8_t(((c) >>  0) & 0xFF))
#define GET_GREEN(c) (uint8_t(((c) >>  8) & 0xFF))
#define GET_BLUE(c)  (uint8_t(((c) >> 16) & 0xFF))
#define GET_ALPHA(c) (uint8_t(((c) >> 24) & 0xFF))

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
	};

public:
	Tesselator(int allotedSize = 0x800000);
	~Tesselator();

	void addOffset(float x, float y, float z);
	void begin();
	void begin(int accessMode);
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
	void offset(float, float, float);
	void setAccessMode(int mode); // sets m_DrawArraysMode
	void tex(float u, float v);
	void vertex(float x, float y, float z);
	void vertexUV(float x, float y, float z, float u, float v);
	void voidBeginAndEndCalls(bool b);

	RenderChunk end(int);

public:
	Vertex* m_pVertices;
	int field_4;
	float m_offsetX;
	float m_offsetY;
	float m_offsetZ;

	float m_nextVtxU;
	float m_nextVtxV;
	uint32_t m_nextVtxColor;

	bool m_bHaveColor;
	bool m_bHaveTex;
	bool field_26;
	bool m_bBlockColor;
	bool field_28;
	int m_nVertices;
	int field_30;
	bool field_34;

	int m_vboCount;
	int field_3C;
	GLuint* m_pVBOs;

	int field_48;
	int m_maxVertices;
	GLuint m_drawArraysMode;
	int m_accessMode;

	std::map<uint32_t, uint32_t> m_VboIdxToRenderChunkID;
};

