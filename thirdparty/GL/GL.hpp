/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "common/Utils.hpp" // needed for M_PI
#include "compat/PlatformDefinitions.h"
#include "GameMods.hpp"

#ifdef USE_NATIVE_ANDROID
	#define USE_GLES
	#include <EGL/egl.h>
#endif

// Disable this on OpenGL ES 2+
#define USE_GL_NORMAL_LIGHTING

//#define MC_GL_DEBUG_OUTPUT

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

#if defined(MC_GL_DEBUG_OUTPUT) && !defined(GL_ARB_debug_output)
#undef MC_GL_DEBUG_OUTPUT
#endif

#ifdef MC_GL_DEBUG_OUTPUT
#ifndef GL_DEBUG_OUTPUT
#define GL_DEBUG_OUTPUT 0x92E0
#endif
typedef void (APIENTRY* DEBUGPROC)(GLenum source,
	GLenum type,
	GLuint id,
	GLenum severity,
	GLsizei length,
	const GLchar* message,
	GLvoid* userParam);
#endif

#ifdef _WIN32
void xglInit();
bool xglInitted();
#endif

#if defined(USE_OPENGL_2_FEATURES) && !defined(_WIN32) && !defined(__DREAMCAST__)

#if GL_VERSION_1_3 || GL_ES_VERSION_1_0
#define xglActiveTexture glActiveTexture
#define xglLoadTransposeMatrixf glLoadTransposeMatrixf
#endif // GL_VERSION_1_3 || GL_ES_VERSION_1_0
#if GL_VERSION_1_5 || GL_ES_VERSION_1_0
#define xglBindBuffer glBindBuffer
#define xglBufferData glBufferData
#define xglGenBuffers glGenBuffers
#define xglDeleteBuffers glDeleteBuffers
#define xglBufferSubData glBufferSubData
#define xglEnableClientState glEnableClientState
#define xglDisableClientState glDisableClientState
#define xglTexCoordPointer glTexCoordPointer
#define xglColorPointer glColorPointer
#define xglNormalPointer glNormalPointer
#define xglVertexPointer glVertexPointer
#define xglDrawArrays glDrawArrays
#endif // GL_VERSION_1_5 || GL_ES_VERSION_1_0
#if GL_VERSION_2_0 || GL_ES_VERSION_1_0
#define xglStencilFuncSeparate glStencilFuncSeparate
#define xglStencilOpSeparate glStencilOpSeparate
#endif // GL_VERSION_2_0 || GL_ES_VERSION_1_0
#ifdef FEATURE_GFX_SHADERS
#if GL_VERSION_2_0 || GL_ES_VERSION_2_0
#define xglUniform1i glUniform1i
#define xglUniform1fv glUniform1fv
#define xglUniform2fv glUniform2fv
#define xglUniform3fv glUniform3fv
#define xglUniform4fv glUniform4fv
#define xglUniform1iv glUniform1iv
#define xglUniform2iv glUniform2iv
#define xglUniform3iv glUniform3iv
#define xglUniform4iv glUniform4iv
#define xglUniformMatrix2fv glUniformMatrix2fv
#define xglUniformMatrix3fv glUniformMatrix3fv
#define xglUniformMatrix4fv glUniformMatrix4fv
#define xglCreateShader glCreateShader
#define xglShaderSource glShaderSource
#define xglCompileShader glCompileShader
#define xglGetShaderiv glGetShaderiv
#define xglGetShaderInfoLog glGetShaderInfoLog
#define xglDeleteShader glDeleteShader
#define xglDeleteProgram glDeleteProgram
#define xglCreateProgram glCreateProgram
#define xglAttachShader glAttachShader
#define xglLinkProgram glLinkProgram
#define xglGetProgramiv glGetProgramiv
#define xglGetProgramInfoLog glGetProgramInfoLog
#define xglVertexAttribPointer glVertexAttribPointer
#define xglUseProgram glUseProgram
#define xglGetActiveUniform glGetActiveUniform
#define xglGetActiveAttrib glGetActiveAttrib
#define xglGetUniformLocation glGetUniformLocation
#define xglGetAttribLocation glGetAttribLocation
#define xglEnableVertexAttribArray glEnableVertexAttribArray
#endif // GL_VERSION_2_0 || GL_ES_VERSION_2_0
#if GL_VERSION_4_1 || GL_ES_VERSION_2_0
#define xglReleaseShaderCompiler glReleaseShaderCompiler
#define xglGetShaderPrecisionFormat glGetShaderPrecisionFormat
#endif // GL_VERSION_4_1 || GL_ES_VERSION_2_0
#endif // FEATURE_GFX_SHADERS
#ifdef MC_GL_DEBUG_OUTPUT
#define xglDebugMessageCallback glDebugMessageCallback
#endif

#else

#if GL_VERSION_1_3 || GL_ES_VERSION_1_0
void xglActiveTexture(GLenum texture);
void xglLoadTransposeMatrixf(const GLfloat* m);
#endif // GL_VERSION_1_3 || GL_ES_VERSION_1_0
#if GL_VERSION_1_5 || GL_ES_VERSION_1_0
void xglBindBuffer(GLenum target, GLuint buffer);
void xglBufferData(GLenum target, GLsizeiptr size, const GLvoid* data, GLenum usage);
void xglGenBuffers(GLsizei num, GLuint* buffers);
void xglDeleteBuffers(GLsizei num, GLuint* buffers);
void xglBufferSubData(GLenum target, GLintptr offset, GLsizeiptr size, const GLvoid* data);
#endif // GL_VERSION_1_5 || GL_ES_VERSION_1_0
#if GL_VERSION_2_0 || GL_ES_VERSION_1_0
void xglStencilFuncSeparate(GLenum face, GLenum func, GLint ref, GLuint mask);
void xglStencilOpSeparate(GLenum face, GLenum sfail, GLenum dpfail, GLenum dppass);
#endif // GL_VERSION_2_0 || GL_ES_VERSION_1_0
#ifdef FEATURE_GFX_SHADERS
#if GL_VERSION_2_0 || GL_ES_VERSION_2_0
void xglUniform1i(GLint location, GLint v0);
void xglUniform1fv(GLint location, GLsizei count, const GLfloat* value);
void xglUniform2fv(GLint location, GLsizei count, const GLfloat* value);
void xglUniform3fv(GLint location, GLsizei count, const GLfloat* value);
void xglUniform4fv(GLint location, GLsizei count, const GLfloat* value);
void xglUniform1iv(GLint location, GLsizei count, const GLint* value);
void xglUniform2iv(GLint location, GLsizei count, const GLint* value);
void xglUniform3iv(GLint location, GLsizei count, const GLint* value);
void xglUniform4iv(GLint location, GLsizei count, const GLint* value);
void xglUniformMatrix2fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
void xglUniformMatrix3fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
void xglUniformMatrix4fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
GLuint xglCreateShader(GLenum type);
void xglShaderSource(GLuint shader, GLsizei count, const GLchar** string, const GLint* length);
void xglCompileShader(GLuint shader);
void xglGetShaderiv(GLuint shader, GLenum pname, GLint* params);
void xglGetShaderInfoLog(GLuint shader, GLsizei bufSize, GLsizei* length, GLchar* infoLog);
void xglDeleteShader(GLuint shader);
void xglDeleteProgram(GLuint program);
GLuint xglCreateProgram();
void xglAttachShader(GLuint program, GLuint shader);
void xglLinkProgram(GLuint program);
void xglGetProgramiv(GLuint program, GLenum pname, GLint* params);
void xglGetProgramInfoLog(GLuint program, GLsizei bufSize, GLsizei* length, GLchar* infoLog);
void xglVertexAttribPointer(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid* pointer);
void xglUseProgram(GLuint program);
void xglGetActiveUniform(GLuint program, GLuint index, GLsizei bufSize, GLsizei* length, GLint* size, GLenum* type, GLchar* name);
void xglGetActiveAttrib(GLuint program, GLuint index, GLsizei bufSize, GLsizei* length, GLint* size, GLenum* type, GLchar* name);
GLint xglGetUniformLocation(GLuint program, const GLchar* name);
GLint xglGetAttribLocation(GLuint program, const GLchar* name);
void xglEnableVertexAttribArray(GLuint index);
#endif // GL_VERSION_2_0 || GL_ES_VERSION_2_0
#if GL_VERSION_4_1 || GL_ES_VERSION_2_0
void xglReleaseShaderCompiler();
void xglGetShaderPrecisionFormat(GLenum shadertype, GLenum precisiontype, GLint* range, GLint* precision);
#endif
#endif // FEATURE_GFX_SHADERS
#ifdef MC_GL_DEBUG_OUTPUT
void xglDebugMessageCallback(DEBUGPROC callback, GLvoid* userParam);
#endif
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
