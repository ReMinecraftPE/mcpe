/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "Cube.hpp"
#include "renderer/GL/GL.hpp"

#define MUL_DEG_TO_RAD (180.0f / float(M_PI))

Cube::Cube(ModelPart* a2, int a3, int a4, float x, float y, float z, int d, int e, int f, float g)
{
	float x1 = x, y1 = y, z1 = z;
	float x2 = x + float(d), y2 = y + float(e), z2 = z + float(f);

	field_29C = x1;
	field_2A0 = y1;
	field_2A4 = z1;
	field_2A8 = x2;
	field_2AC = y2;
	field_2B0 = z2;

	if (a2->m_bMirror)
		std::swap(x1, x2);
	
	x1 -= g;
	y1 -= g;
	z1 -= g;
	x2 += g;
	y2 += g;
	z2 += g;

	m_verts[0] = VertexPT(x1, y1, z1, 0.0f, 0.0f);
	m_verts[1] = VertexPT(x2, y1, z1, 0.0f, 8.0f);
	m_verts[2] = VertexPT(x2, y2, z1, 8.0f, 8.0f);
	m_verts[3] = VertexPT(x1, y2, z1, 8.0f, 0.0f);
	m_verts[4] = VertexPT(x1, y1, z2, 0.0f, 0.0f);
	m_verts[5] = VertexPT(x2, y1, z2, 0.0f, 8.0f);
	m_verts[6] = VertexPT(x2, y2, z2, 8.0f, 8.0f);
	m_verts[7] = VertexPT(x1, y2, z2, 8.0f, 0.0f);
	
	int m = a3, n = a4;

	m_faces[0] = PolygonQuad(&m_verts[5], &m_verts[1], &m_verts[2], &m_verts[6], m + f + d,     n + f, m + f + d + f,     n + f + e);     // x2 face
	m_faces[1] = PolygonQuad(&m_verts[0], &m_verts[4], &m_verts[7], &m_verts[3], m,             n + f, m + f,             n + f + e);     // x1 face
	m_faces[2] = PolygonQuad(&m_verts[5], &m_verts[4], &m_verts[0], &m_verts[1], m + f,         n,     m + f + d,         n + f);         // up face
	m_faces[3] = PolygonQuad(&m_verts[2], &m_verts[3], &m_verts[7], &m_verts[6], m + f + d,     n,     m + f + d + d,     n + f);         // down face*
	m_faces[4] = PolygonQuad(&m_verts[1], &m_verts[0], &m_verts[3], &m_verts[2], m + f,         n + f, m + f + d,         n + f + e);     // z1 face
	m_faces[5] = PolygonQuad(&m_verts[4], &m_verts[5], &m_verts[6], &m_verts[7], m + f + d + f, n + f, m + f + d + f + d, n + f + e);     // z2 face
    
    // *N.B. The original game specifies the vertex ordering as 2, 3, 7, 6, but that renders the back side of the cow upside down.
    // This might not be proper form for the face, but we're disabling culling anyway so who cares.
    
    // Despite the updated vertex ordering, the textures I have are causing the cow's back side
    // to be rendered upside down. Perhaps it's a texture issue, not a vertex ordering issue.
    // Reverting to b1.2_02 defaults for now, as it corrects the problem on my end.
    // - Brent

	if (a2->m_bMirror)
	{
		for (int i = 0; i < 6; i++)
			m_faces[i].mirror();
	}
}

void Cube::compile(Tesselator& t, float scale)
{
	for (int i = 0; i < 6; i++)
		m_faces[i].render(t, scale);
}

void Cube::setId(const std::string& s)
{
	m_id = s;
}
