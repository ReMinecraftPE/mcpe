#pragma once

#include "thirdparty/GL/GL.hpp"

int glhInvertMatrixf2(float* m, float* out);
int glhProjectf(float objx, float objy, float objz, float* modelview, float* projection, int* viewport, float* windowCoordinate);
int glhUnProjectf(float winx, float winy, float winz, float* modelview, float* projection, int* viewport, float* objectCoordinate);

void MultiplyMatrices4by4OpenGL_FLOAT(float* result, float* matrix1, float* matrix2);
void MultiplyMatrixByVector4by4OpenGL_FLOAT(float* resultvector, const float* matrix, const float* pvector);
