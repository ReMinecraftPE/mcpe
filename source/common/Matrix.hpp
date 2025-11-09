/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include <cstring>
#include "Mth.hpp"
#include "thirdparty/GL/GL.hpp"

class MatrixGL
{
public:
	MatrixGL();         // create an empty matrix
	MatrixGL(float a);  // create an identity matrix
	MatrixGL(float* p); // load matrix from memory
	MatrixGL(float a, float b, float c, float d, float e, float f, float g, float h, float i, float j, float k, float l, float m, float n, float o, float p);
	void fetchGL(GLenum pname);

	friend MatrixGL operator*(const MatrixGL& a, const MatrixGL& b);

public:
	float c[16];
};

MatrixGL operator*(const MatrixGL& a, const MatrixGL& b);

