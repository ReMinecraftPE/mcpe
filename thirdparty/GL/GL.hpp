/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include <common/Utils.hpp> // it includes GL/gl.h

#if defined(ANDROID) && !defined(USE_SDL)
	#define USE_GLES
	#include <EGL/egl.h>
#endif

#ifdef USE_GLES1_COMPATIBILITY_LAYER
	#define USE_GLES // GLES or its compatibility layer.
#endif

#ifdef USE_GLES
	#include <GLES/gl.h>
	#define GL_QUADS 0x7

	#define USE_OPENGL_2_FEATURES

	#include <cmath>

	// https://cgit.freedesktop.org/mesa/glu/tree/src/libutil/project.c
	static inline void __gluMakeIdentityf(GLfloat m[16]) {
		m[0 + 4 * 0] = 1; m[0 + 4 * 1] = 0; m[0 + 4 * 2] = 0; m[0 + 4 * 3] = 0;
		m[1 + 4 * 0] = 0; m[1 + 4 * 1] = 1; m[1 + 4 * 2] = 0; m[1 + 4 * 3] = 0;
		m[2 + 4 * 0] = 0; m[2 + 4 * 1] = 0; m[2 + 4 * 2] = 1; m[2 + 4 * 3] = 0;
		m[3 + 4 * 0] = 0; m[3 + 4 * 1] = 0; m[3 + 4 * 2] = 0; m[3 + 4 * 3] = 1;
	}
	static inline void gluPerspective(GLfloat fovy, GLfloat aspect, GLfloat zNear, GLfloat zFar) {
		GLfloat m[4][4];
		float sine, cotangent, deltaZ;
		float radians = fovy / 2 * M_PI / 180;

		deltaZ = zFar - zNear;
		sine = sin(radians);
		if ((deltaZ == 0) || (sine == 0) || (aspect == 0)) {
			return;
		}
		cotangent = cosf(radians) / sine;

		__gluMakeIdentityf(&m[0][0]);
		m[0][0] = cotangent / aspect;
		m[1][1] = cotangent;
		m[2][2] = -(zFar + zNear) / deltaZ;
		m[2][3] = -1;
		m[3][2] = -2 * zNear * zFar / deltaZ;
		m[3][3] = 0;
		glMultMatrixf(&m[0][0]);
	}

	#define USE_GL_ORTHO_F

#else
	#include <GL/gl.h>
	#include <GL/glu.h>

	#ifdef _WIN32
		#pragma comment(lib, "opengl32.lib")
		#pragma comment(lib, "glu32.lib")
	#endif

	#ifdef __APPLE__
		#include <OpenGL/glu.h>
	#else
		#include <GL/glu.h>
	#endif

	#ifdef USE_SDL
		#define USE_OPENGL_2_FEATURES

		#define GL_GLEXT_PROTOTYPES
		#include "thirdparty/SDL2/SDL_opengl.h"

		#ifndef _WIN32
			#include <SDL2/SDL_opengl_glext.h>
		#endif

		#ifndef __APPLE__
			#include <GL/glext.h> // it'll include from a different dir, namely thirdparty/GL/glext.h
		#endif
	#elif !defined __APPLE__
		#include <GL/glext.h> // it'll include from a different dir, namely thirdparty/GL/glext.h
	#endif

	// use our macro for glOrtho
#endif

#ifdef USE_OPENGL_2_FEATURES

#define xglBindBuffer glBindBuffer
#define xglBufferData glBufferData
#define xglGenBuffers glGenBuffers
#define xglDeleteBuffers glDeleteBuffers
#define xglEnableClientState glEnableClientState
#define xglDisableClientState glDisableClientState
#define xglTexCoordPointer glTexCoordPointer
#define xglColorPointer glColorPointer
#define xglVertexPointer glVertexPointer
#define xglDrawArrays glDrawArrays

#else

void xglInit();
bool xglInitted();

void xglBindBuffer(GLenum target, GLuint buffer);
void xglBufferData(GLenum target, GLsizeiptr size, const GLvoid* data, GLenum usage);
void xglGenBuffers(GLsizei num, GLuint* buffers);
void xglDeleteBuffers(GLsizei num, GLuint* buffers);
void xglOrthof(GLfloat left, GLfloat right, GLfloat bottom, GLfloat top, GLfloat nearpl, GLfloat farpl);
void xglSwapIntervalEXT(int interval);
void xglEnableClientState(GLenum _array);
void xglDisableClientState(GLenum _array);
void xglTexCoordPointer(GLint size, GLenum type, GLsizei stride, const GLvoid* pointer);
void xglColorPointer(GLint size, GLenum type, GLsizei stride, const GLvoid* pointer);
void xglVertexPointer(GLint size, GLenum type, GLsizei stride, const GLvoid* pointer);
void xglDrawArrays(GLenum mode, GLint first, GLsizei count);

#endif

#ifdef _WIN32
// Win32 defines xglOrthof as a regular function
#elif defined USE_GL_ORTHO_F
#define xglOrthof glOrthof
#else
#define xglOrthof(left, right, bottom, top, nearpl, farpl) glOrtho((GLdouble) (left), (GLdouble) (right), (GLdouble) (bottom), (GLdouble) (top), (GLdouble) (nearpl), (GLdouble) (farpl))
#endif
