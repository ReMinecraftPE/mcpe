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
	void setPos(float x, float y, float z);
	void setTexOffs(int a, int b);
	void translateTo(float f);
	void setBrightness(float f);

public:
	float m_posX;
	float m_posY;
	float m_posZ;
	float field_C;
	float field_10;
	float field_14;
	bool field_18;
	bool field_19;
	bool field_1A;
	VertexPT m_verts[8];
	PolygonQuad m_faces[6];
	int field_2B4;
	int field_2B8;
	bool m_bCompiled;
	int field_2C0;
	GLuint m_buffer;
	float m_brightness;

	static float c;
};

