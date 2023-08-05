/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "Cube.hpp"

float Cube::c = 180.0f / float(M_PI);

Cube::Cube(int a, int b)
{
	field_2B4 = a;
	field_2B8 = b;
}

void Cube::addBox(float a, float b, float c, int d, int e, int f)
{
	addBox(a, b, c, d, e, f, 0.0f);
}

void Cube::addBox(float x, float y, float z, int d, int e, int f, float g)
{
	float x1 = x, y1 = y, z1 = z;
	float x2 = x + float(d), y2 = y + float(e), z2 = z + float(f);
	x1 -= g;
	y1 -= g;
	z1 -= g;
	x2 += g;
	y2 += g;
	z2 += g;

	if (field_18)
		std::swap(x1, x2);

	m_verts[0] = VertexPT(x1, y1, z1, 0.0f, 0.0f);
	m_verts[1] = VertexPT(x2, y1, z1, 0.0f, 8.0f);
	m_verts[2] = VertexPT(x2, y2, z1, 8.0f, 8.0f);
	m_verts[3] = VertexPT(x1, y2, z1, 8.0f, 0.0f);
	m_verts[4] = VertexPT(x1, y1, z2, 0.0f, 0.0f);
	m_verts[5] = VertexPT(x2, y1, z2, 0.0f, 8.0f);
	m_verts[6] = VertexPT(x2, y2, z2, 8.0f, 8.0f);
	m_verts[7] = VertexPT(x1, y2, z2, 8.0f, 0.0f);

	int m = field_2B4, n = field_2B8;

	m_faces[0] = PolygonQuad(&m_verts[5], &m_verts[1], &m_verts[2], &m_verts[6], m + f + d,     n + f, m + f + d + f,     n + f + e);     // x2 face
	m_faces[1] = PolygonQuad(&m_verts[0], &m_verts[4], &m_verts[7], &m_verts[3], m,             n + f, m + f,             n + f + e);     // x1 face
	m_faces[2] = PolygonQuad(&m_verts[5], &m_verts[4], &m_verts[0], &m_verts[1], m + f,         n,     m + f + d,         n + f);         // up face
	m_faces[3] = PolygonQuad(&m_verts[2], &m_verts[3], &m_verts[7], &m_verts[6], m + f + d,     n,     m + f + d + d,     n + f);         // down face
	m_faces[4] = PolygonQuad(&m_verts[1], &m_verts[0], &m_verts[3], &m_verts[2], m + f,         n + f, m + f + d,         n + f + e);     // z1 face
	m_faces[5] = PolygonQuad(&m_verts[4], &m_verts[5], &m_verts[6], &m_verts[7], m + f + d + f, n + f, m + f + d + f + d, n + f + e);     // z2 face

#ifdef ENH_ENTITY_SHADING
	m_faces[0].setColor(0.6f, 0.6f, 0.6f);
	m_faces[1].setColor(0.6f, 0.6f, 0.6f);
	m_faces[4].setColor(0.8f, 0.8f, 0.8f);
	m_faces[5].setColor(0.8f, 0.8f, 0.8f);
	m_faces[3].setColor(0.5f, 0.5f, 0.5f);
#endif

	if (field_18)
	{
		for (int i = 0; i < 6; i++)
			m_faces[i].mirror();
	}
}

void Cube::compile(float f)
{
	xglDeleteBuffers(1, &m_buffer);
	xglGenBuffers(1, &m_buffer);

	Tesselator& t = Tesselator::instance;
	t.begin();

	for (int i = 0; i < 6; i++)
		m_faces[i].render(t, f, m_buffer);

	t.end(m_buffer);
	m_bCompiled = true;
}

void Cube::draw()
{
#ifdef ENH_ENTITY_SHADING
	drawArrayVTC(m_buffer, 36, sizeof(Tesselator::Vertex));
#else
	drawArrayVT(m_buffer, 36, sizeof(Tesselator::Vertex));
#endif
}

void Cube::drawSlow(float f)
{
	Tesselator& t = Tesselator::instance;
	t.begin();

	for (int i = 0; i < 6; i++)
		m_faces[i].render(t, f, m_buffer);

	t.draw();
}

void Cube::render(float f)
{
	if (field_1A)
		return;

	if (!field_19)
		return;

	if (!m_bCompiled)
		compile(f);

	if (field_C == 0.0f && field_10 == 0.0f && field_14 == 0.0f)
	{
		if (m_posX == 0.0f && m_posY == 0 && m_posZ == 0)
		{
			draw();
			return;
		}

		glTranslatef( m_posX * f,  m_posY * f,  m_posZ * f);
		draw();
		glTranslatef(-m_posX * f, -m_posY * f, -m_posZ * f);
		return;
	}

	glPushMatrix();

	glTranslatef(m_posX * f, m_posY * f, m_posZ * f);
	if (field_14 != 0.0f) glRotatef(field_14 * c, 0.0f, 0.0f, 1.0f);
	if (field_10 != 0.0f) glRotatef(field_10 * c, 0.0f, 1.0f, 0.0f);
	if (field_C  != 0.0f) glRotatef(field_C  * c, 1.0f, 0.0f, 0.0f);
	draw();

	glPopMatrix();
}

void Cube::renderRollable(float f)
{
	if (field_1A)
		return;

	if (!field_19)
		return;

	if (!m_bCompiled)
		compile(f);

	glPushMatrix();

	glTranslatef(m_posX * f, m_posY * f, m_posZ * f);
	if (field_14 != 0.0f) glRotatef(field_14 * c, 0.0f, 0.0f, 1.0f);
	if (field_10 != 0.0f) glRotatef(field_10 * c, 0.0f, 1.0f, 0.0f);
	if (field_C  != 0.0f) glRotatef(field_C  * c, 1.0f, 0.0f, 0.0f);
	draw();

	glPopMatrix();
}

void Cube::renderHorrible(float f)
{
	if (field_1A)
		return;

	if (!field_19)
		return;

	if (field_C == 0.0f && field_10 == 0.0f && field_14 == 0.0f)
	{
		if (m_posX == 0.0f && m_posY == 0 && m_posZ == 0)
		{
			drawSlow(f);
			return;
		}

		glTranslatef( m_posX * f,  m_posY * f,  m_posZ * f);
		drawSlow(f);
		glTranslatef(-m_posX * f, -m_posY * f, -m_posZ * f);
		return;
	}

	glPushMatrix();

	glTranslatef(m_posX * f, m_posY * f, m_posZ * f);
	if (field_14 != 0.0f) glRotatef(field_14 * c, 0.0f, 0.0f, 1.0f);
	if (field_10 != 0.0f) glRotatef(field_10 * c, 0.0f, 1.0f, 0.0f);
	if (field_C  != 0.0f) glRotatef(field_C  * c, 1.0f, 0.0f, 0.0f);
	drawSlow(f);

	glPopMatrix();
}

void Cube::translateTo(float f)
{
	if (field_1A)
		return;

	if (!field_19)
		return;

	if (field_C == 0.0f && field_10 == 0.0f && field_14 == 0.0f)
	{
		if (m_posX == 0.0f && m_posY == 0 && m_posZ == 0)
			return;

		glTranslatef( m_posX * f,  m_posY * f,  m_posZ * f);
		return;
	}

	glTranslatef(m_posX * f, m_posY * f, m_posZ * f);
	if (field_14 != 0.0f) glRotatef(field_14 * c, 0.0f, 0.0f, 1.0f);
	if (field_10 != 0.0f) glRotatef(field_10 * c, 0.0f, 1.0f, 0.0f);
	if (field_C  != 0.0f) glRotatef(field_C  * c, 1.0f, 0.0f, 0.0f);
}

void Cube::setPos(float x, float y, float z)
{
	m_posX = x;
	m_posY = y;
	m_posZ = z;
}

void Cube::setTexOffs(int a, int b)
{
	field_2B4 = a;
	field_2B8 = b;
}
