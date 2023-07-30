#pragma once

#ifdef USE_OPENGL_2

#define xglBindBuffer glBindBuffer
#define xglBufferData glBufferData
#define xglGenBuffers glGenBuffers

#else

#include "Utils.hpp"
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glext.h> // it'll include from a different dir, namely thirdparty/GL/glext.h

void xglInit();
bool xglInitted();

void xglBindBuffer(GLenum target, GLuint buffer);
void xglBufferData(GLenum target, GLsizeiptr size, const GLvoid* data, GLenum usage);
void xglGenBuffers(GLsizei num, GLuint* buffers);
void xglDeleteBuffers(GLsizei num, GLuint* buffers);
void xglOrthof(GLfloat left, GLfloat right, GLfloat bottom, GLfloat top, GLfloat nearpl, GLfloat farpl);
void xglSwapIntervalEXT(int interval);

// @TODO: not the right place, but er, it's ok
void drawArrayVT(GLuint buffer, int count, int stride);
void drawArrayVTC(GLuint buffer, int count, int stride);

#endif
