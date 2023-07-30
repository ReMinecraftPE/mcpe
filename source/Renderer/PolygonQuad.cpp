/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "PolygonQuad.hpp"

PolygonQuad::PolygonQuad(VertexPT* a, VertexPT* b, VertexPT* c, VertexPT* d)
{
	m_verts[0] = *a;
	m_verts[1] = *b;
	m_verts[2] = *c;
	m_verts[3] = *d;
}

PolygonQuad::PolygonQuad(VertexPT* a, VertexPT* b, VertexPT* c, VertexPT* d, float u1, float v1, float u2, float v2)
{
	m_verts[0] = *a;
	m_verts[1] = *b;
	m_verts[2] = *c;
	m_verts[3] = *d;

	m_verts[0].setUV(u2, v1);
	m_verts[1].setUV(u1, v1);
	m_verts[2].setUV(u1, v2);
	m_verts[3].setUV(u2, v2);
}

PolygonQuad::PolygonQuad(VertexPT* a, VertexPT* b, VertexPT* c, VertexPT* d, int u1i, int v1i, int u2i, int v2i)
{
	constexpr float C_F1 = 0.0015625f, C_F2 = 0.003125f;
	m_verts[0] = *a;
	m_verts[1] = *b;
	m_verts[2] = *c;
	m_verts[3] = *d;

	m_verts[0].setUV(float(u2i) / 64.0f - C_F1, float(v1i) / 32.0f + C_F2);
	m_verts[1].setUV(float(u1i) / 64.0f + C_F1, float(v1i) / 32.0f + C_F2);
	m_verts[2].setUV(float(u1i) / 64.0f + C_F1, float(v2i) / 32.0f - C_F2);
	m_verts[3].setUV(float(u2i) / 64.0f - C_F1, float(v2i) / 32.0f - C_F2);
}

void PolygonQuad::flipNormal()
{
	m_bFlipNormals = true;
}

void PolygonQuad::mirror()
{
	std::swap(m_verts[0], m_verts[3]);
	std::swap(m_verts[1], m_verts[2]);
}

void PolygonQuad::render(Tesselator& t, float scale, int unused)
{
	for (int i = 0; i < 4; i++)
	{
		VertexPT& v = m_verts[i];
	#ifdef ENH_ENTITY_SHADING
		t.color(m_color);
	#endif
		t.vertexUV(scale * v.x, scale * v.y, scale * v.z, v.u, v.v);
	}
}

#ifdef ENH_ENTITY_SHADING
void PolygonQuad::setColor(float r, float g, float b)
{
	int ru = int(255 * r), gu = int(255 * g), bu = int(255 * g);
	m_color = bu | (gu << 8) | (ru << 16);
}
#endif
