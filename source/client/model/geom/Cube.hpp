/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "PolygonQuad.hpp"
#include "ModelPart.hpp"

class ModelPart;

class Cube
{
public:
	Cube(ModelPart*, const TextureOffset& texOffs, float, float, float, int, int, int, float);

	void compile(Tesselator& t, float scale);
	void setId(const std::string& s);

public:
	VertexPT m_verts[8];
	PolygonQuad m_faces[6];
	std::string m_id;
};

