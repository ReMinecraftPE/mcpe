/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include <cstdint>
#include <map>
#include "compat/GL.hpp"
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
	Vertex* m_pVertices = nullptr;
	int field_4 = 0;
	float m_offsetX = 0.0f;
	float m_offsetY = 0.0f;
	float m_offsetZ = 0.0f;

	float m_nextVtxU = 0;
	float m_nextVtxV = 0;
	uint32_t m_nextVtxColor = 0;

	bool m_bHaveColor = false;
	bool m_bHaveTex = false;
	bool field_26 = false;
	bool m_bBlockColor = false;
	bool field_28 = false;
	int field_2C = 0;
	int field_30 = 0;
	bool field_34 = false;

	int m_vboCount = 1024;
	int field_3C = -1;
	GLuint* m_pVBOs = nullptr;

	int field_48 = 0;
	int m_maxVertices = 0;
	GLuint m_drawArraysMode = 0;
	int m_accessMode = 0;

	std::map<uint32_t, uint32_t> m_VboIdxToRenderChunkID;
};

