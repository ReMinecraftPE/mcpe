/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "PolygonQuad.hpp"

class Cube
{
public:
	Cube(int, int);

	void addBox(float a, float b, float c, int d, int e, int f);
	void addBox(float a, float b, float c, int d, int e, int f, float g);
	// @TODO: void addTexBox(float a, float b, float c, int d, int e, int f, int g); -- No xrefs
	void compile(float f);
	void draw();
	void drawSlow(float f);
	void render(float f);
	void renderHorrible(float f);
	void renderRollable(float f);
	void setPos(float x, float y, float z);
	void setTexOffs(int a, int b);
	void translateTo(float f);

public:
	float m_posX = 0.0f;
	float m_posY = 0.0f;
	float m_posZ = 0.0f;
	float field_C = 0.0f;
	float field_10 = 0.0f;
	float field_14 = 0.0f;
	bool field_18 = false;
	bool field_19 = true;
	bool field_1A = false;
	VertexPT m_verts[8];
	PolygonQuad m_faces[6];
	int field_2B4 = 0;
	int field_2B8 = 0;
	bool m_bCompiled = false;
	int field_2C0 = 0;
	GLuint m_buffer = 0;

	static float c;
};

