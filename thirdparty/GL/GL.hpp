/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include <common/Utils.hpp> // it includes GL/gl.h
#include <compat/PlatformDefinitions.h>

#ifdef USE_NATIVE_ANDROID
	#define USE_GLES
	#include <EGL/egl.h>
#endif

// Disable this on OpenGL ES 2+
#define USE_GL_NORMAL_LIGHTING

#ifdef USE_GLES
	#if MC_PLATFORM_IOS
		 #import <OpenGLES/ES1/gl.h>
		 #import <OpenGLES/ES1/glext.h>

		#define glFogi glFogx
	#else
		#include <GLES/gl.h>
	#endif
	#define GL_QUADS 0x7

	#define USE_OPENGL_2_FEATURES

	#include <cmath>

	#define USE_GL_ORTHO_F

	// https://discourse.libsdl.org/t/opengl-es2-support-on-windows/20177/10
	// float on GLES for performance reasons (mobile hardware) rather than double precision on GL
	#if GL_ES_VERSION_2_0
		#define glClearColor glClearColorf
	#endif
	#define glClearDepth glClearDepthf
	#define glDepthRange glDepthRangef
#else
	#ifdef USE_SDL
		#define USE_OPENGL_2_FEATURES

		#define GL_GLEXT_PROTOTYPES
		#include "thirdparty/SDL/SDL_opengl.h"

		#if !defined(_WIN32) && SDL_MAJOR_VERSION == 2
			#include <SDL2/SDL_opengl_glext.h>
		#endif
	#else
		#ifdef __APPLE__
			#include <OpenGL/gl.h>
			#include <OpenGL/glu.h>
		#else
			#include <GL/gl.h>
			#include <GL/glu.h>
			#include <GL/glext.h> // it'll include from a different dir, namely thirdparty/GL/glext.h
		#endif

		#ifdef _WIN32
			#pragma comment(lib, "opengl32.lib")
			#pragma comment(lib, "glu32.lib")
		#endif
	#endif

	// use our macro for glOrtho
#endif

#if defined(USE_GLES) || defined(USE_SDL)
	// https://cgit.freedesktop.org/mesa/glu/tree/src/libutil/project.c
	inline void __gluMakeIdentityf(GLfloat m[16]) {
		m[0 + 4 * 0] = 1; m[0 + 4 * 1] = 0; m[0 + 4 * 2] = 0; m[0 + 4 * 3] = 0;
		m[1 + 4 * 0] = 0; m[1 + 4 * 1] = 1; m[1 + 4 * 2] = 0; m[1 + 4 * 3] = 0;
		m[2 + 4 * 0] = 0; m[2 + 4 * 1] = 0; m[2 + 4 * 2] = 1; m[2 + 4 * 3] = 0;
		m[3 + 4 * 0] = 0; m[3 + 4 * 1] = 0; m[3 + 4 * 2] = 0; m[3 + 4 * 3] = 1;
	}
	inline void gluPerspective(GLfloat fovy, GLfloat aspect, GLfloat zNear, GLfloat zFar) {
		GLfloat m[4][4];
		float sine, cotangent, deltaZ;
		float radians = fovy / 2.0f * float(M_PI) / 180.0f;

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
#endif

#ifdef _WIN32
void xglInit();
bool xglInitted();
#endif

#if defined(USE_OPENGL_2_FEATURES) && !defined(_WIN32) && !defined(__DREAMCAST__)

#define xglBindBuffer glBindBuffer
#define xglBufferData glBufferData
#define xglGenBuffers glGenBuffers
#define xglDeleteBuffers glDeleteBuffers
#define xglEnableClientState glEnableClientState
#define xglDisableClientState glDisableClientState
#define xglTexCoordPointer glTexCoordPointer
#define xglColorPointer glColorPointer
#define xglNormalPointer glNormalPointer
#define xglVertexPointer glVertexPointer
#define xglDrawArrays glDrawArrays

#else

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
void xglNormalPointer(GLenum type, GLsizei stride, const GLvoid* pointer);
void xglVertexPointer(GLint size, GLenum type, GLsizei stride, const GLvoid* pointer);
void xglDrawArrays(GLenum mode, GLint first, GLsizei count);

#endif

#if defined(_WIN32) && !defined(USE_SDL)
// Win32 defines xglOrthof as a regular function
#elif defined USE_GL_ORTHO_F
#define xglOrthof glOrthof
#else
#define xglOrthof(left, right, bottom, top, nearpl, farpl) glOrtho((GLdouble) (left), (GLdouble) (right), (GLdouble) (bottom), (GLdouble) (top), (GLdouble) (nearpl), (GLdouble) (farpl))
#endif
