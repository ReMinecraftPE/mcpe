#pragma once

int glhInvertMatrixf2(float* m, float* out);
int glhUnProjectf(float winx, float winy, float winz, const float* modelview, const float* projection, int* viewport, float* objectCoordinate);

void MultiplyMatrices4by4OpenGL_FLOAT(float* result, const float* matrix1, const float* matrix2);
void MultiplyMatrixByVector4by4OpenGL_FLOAT(float* resultvector, const float* matrix, const float* pvector);
