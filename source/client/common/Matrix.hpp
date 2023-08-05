/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include <cstring>
#include "Mth.hpp"
#include "compat/GL.hpp"

class Matrix
{
public:
	Matrix();         // create an empty matrix
	Matrix(float a);  // create an identity matrix
	Matrix(float* p); // load matrix from memory
	Matrix(float a, float b, float c, float d, float e, float f, float g, float h, float i, float j, float k, float l, float m, float n, float o, float p);
	void fetchGL(GLenum pname);

	friend Matrix operator*(const Matrix& a, const Matrix& b);

public:
	float c[16] = { 0.0f };
};

Matrix operator*(const Matrix& a, const Matrix& b);

