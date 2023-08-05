/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "Matrix.hpp"

Matrix::Matrix()
{
	memset(c, 0, sizeof c);
}

Matrix::Matrix(float x)
{
	memset(c, 0, sizeof c);
	c[0] = c[4 * 1 + 1] = c[4 * 2 + 2] = c[4 * 3 + 3] = x;
}

Matrix::Matrix(float* p)
{
	memcpy(c, p, sizeof c);
}

Matrix::Matrix(float a, float b, float q, float d, float e, float f, float g, float h, float i, float j, float k, float l, float m, float n, float o, float p)
{
	c[0] = a, c[1] = b, c[2] = q, c[3] = d, c[4] = e, c[5] = f, c[6] = g, c[7] = h, c[8] = i, c[9] = j, c[10] = k, c[11] = l, c[12] = m, c[13] = n, c[14] = o, c[15] = p;
}

void Matrix::fetchGL(GLenum pname)
{
	glGetFloatv(pname, c);
}

Matrix operator*(const Matrix& a, const Matrix& b)
{
	Matrix result;

	//this is ugly
	result.c[0]  = a.c[0]  * b.c[0] + a.c[1]  * b.c[4] + a.c[2]  * b.c[8]  + a.c[3]  * b.c[12];
	result.c[1]  = a.c[0]  * b.c[1] + a.c[1]  * b.c[5] + a.c[2]  * b.c[9]  + a.c[3]  * b.c[13];
	result.c[2]  = a.c[0]  * b.c[2] + a.c[1]  * b.c[6] + a.c[2]  * b.c[10] + a.c[3]  * b.c[14];
	result.c[3]  = a.c[0]  * b.c[3] + a.c[1]  * b.c[7] + a.c[2]  * b.c[11] + a.c[3]  * b.c[15];
	result.c[4]  = a.c[4]  * b.c[0] + a.c[5]  * b.c[4] + a.c[6]  * b.c[8]  + a.c[7]  * b.c[12];
	result.c[5]  = a.c[4]  * b.c[1] + a.c[5]  * b.c[5] + a.c[6]  * b.c[9]  + a.c[7]  * b.c[13];
	result.c[6]  = a.c[4]  * b.c[2] + a.c[5]  * b.c[6] + a.c[6]  * b.c[10] + a.c[7]  * b.c[14];
	result.c[7]  = a.c[4]  * b.c[3] + a.c[5]  * b.c[7] + a.c[6]  * b.c[11] + a.c[7]  * b.c[15];
	result.c[8]  = a.c[8]  * b.c[0] + a.c[9]  * b.c[4] + a.c[10] * b.c[8]  + a.c[11] * b.c[12];
	result.c[9]  = a.c[8]  * b.c[1] + a.c[9]  * b.c[5] + a.c[10] * b.c[9]  + a.c[11] * b.c[13];
	result.c[10] = a.c[8]  * b.c[2] + a.c[9]  * b.c[6] + a.c[10] * b.c[10] + a.c[11] * b.c[14];
	result.c[11] = a.c[8]  * b.c[3] + a.c[9]  * b.c[7] + a.c[10] * b.c[11] + a.c[11] * b.c[15];
	result.c[12] = a.c[12] * b.c[0] + a.c[13] * b.c[4] + a.c[14] * b.c[8]  + a.c[15] * b.c[12];
	result.c[13] = a.c[12] * b.c[1] + a.c[13] * b.c[5] + a.c[14] * b.c[9]  + a.c[15] * b.c[13];
	result.c[14] = a.c[12] * b.c[2] + a.c[13] * b.c[6] + a.c[14] * b.c[10] + a.c[15] * b.c[14];
	result.c[15] = a.c[12] * b.c[3] + a.c[13] * b.c[7] + a.c[14] * b.c[11] + a.c[15] * b.c[15];

	return result;
}
