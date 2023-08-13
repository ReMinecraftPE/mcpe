/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "GL.hpp"
#include <unordered_map>

HWND GetHWND();
extern LPCTSTR g_GameTitle;

#define USE_HARDWARE_GL_BUFFERS
#ifdef USE_HARDWARE_GL_BUFFERS
PFNGLBINDBUFFERPROC p_glBindBuffer;
PFNGLBUFFERDATAPROC p_glBufferData;
PFNGLGENBUFFERSPROC p_glGenBuffers;
PFNGLDELETEBUFFERSPROC p_glDeleteBuffers;
#endif

// this is stupidly hacky
typedef BOOL(WINAPI* PFNWGLSWAPINTERVALEXTPROC) (int interval);

PFNWGLSWAPINTERVALEXTPROC p_wglSwapIntervalEXT;

bool xglInitted()
{
#ifdef USE_HARDWARE_GL_BUFFERS
	return p_glBindBuffer && p_glBufferData && p_glGenBuffers && p_glDeleteBuffers;
#else
	return true;
#endif
}

void xglInit()
{
#ifdef USE_HARDWARE_GL_BUFFERS
	p_glBindBuffer = (PFNGLBINDBUFFERPROC)wglGetProcAddress("glBindBuffer");
	p_glBufferData = (PFNGLBUFFERDATAPROC)wglGetProcAddress("glBufferData");
	p_glGenBuffers = (PFNGLGENBUFFERSPROC)wglGetProcAddress("glGenBuffers");
	p_glDeleteBuffers = (PFNGLDELETEBUFFERSPROC)wglGetProcAddress("glDeleteBuffers");
#endif

	p_wglSwapIntervalEXT = (PFNWGLSWAPINTERVALEXTPROC)wglGetProcAddress("wglSwapIntervalEXT");

#ifdef USE_HARDWARE_GL_BUFFERS
	if (!xglInitted())
		MessageBox(GetHWND(), TEXT("Error initializing GL extensions. Update your graphics drivers!"), g_GameTitle, MB_OK);
#endif
}

#ifdef USE_HARDWARE_GL_BUFFERS
void xglBindBuffer(GLenum target, GLuint buffer)
{
	p_glBindBuffer(target, buffer);
}

void xglBufferData(GLenum target, GLsizeiptr size, const GLvoid* data, GLenum usage)
{
	p_glBufferData(target, size, data, usage);
}

void xglGenBuffers(GLsizei num, GLuint* buffers)
{
	p_glGenBuffers(num, buffers);
}

void xglDeleteBuffers(GLsizei num, GLuint* buffers)
{
	p_glDeleteBuffers(num, buffers);
}

void xglEnableClientState(GLenum _array)
{
	glEnableClientState(_array);
}

void xglDisableClientState(GLenum _array)
{
	glDisableClientState(_array);
}

void xglTexCoordPointer(GLint size, GLenum type, GLsizei stride, const GLvoid* pointer)
{
	glTexCoordPointer(size, type, stride, pointer);
}

void xglColorPointer(GLint size, GLenum type, GLsizei stride, const GLvoid* pointer)
{
	glColorPointer(size, type, stride, pointer);
}

void xglVertexPointer(GLint size, GLenum type, GLsizei stride, const GLvoid* pointer)
{
	glVertexPointer(size, type, stride, pointer);
}

void xglDrawArrays(GLenum mode, GLint first, GLsizei count)
{
	glDrawArrays(mode, first, count);
}
#endif

void xglOrthof(GLfloat left, GLfloat right, GLfloat bottom, GLfloat top, GLfloat nearpl, GLfloat farpl)
{
	return glOrtho(GLdouble(left), GLdouble(right), GLdouble(bottom), GLfloat(top), GLdouble(nearpl), GLdouble(farpl));
}

void xglSwapIntervalEXT(int interval)
{
	if (!p_wglSwapIntervalEXT)
		return;

	p_wglSwapIntervalEXT(interval);
}

#ifndef USE_HARDWARE_GL_BUFFERS

// ** Software based emulation of OpenGL vertex buffers.

struct GLBuffer
{
	GLvoid* m_pBufferData;
	GLsizei m_bufferSize;
	GLenum  m_usage; // as passed into glBufferData

	GLBuffer()
	{
		m_pBufferData = nullptr;
		m_bufferSize = 0;
		m_usage = 0;
	}
};

std::unordered_map<int, GLBuffer*> g_GLBuffers;
GLBuffer* g_pCurrentlyBoundGLBuffer = nullptr;
int g_NextGLBufferID;

void xglGenBuffers(GLsizei num, GLuint* buffers)
{
	for (GLsizei i = 0; i < num; i++)
	{
		*buffers++ = ++g_NextGLBufferID;
		g_GLBuffers[g_NextGLBufferID] = new GLBuffer;
	}
}

void xglBindBuffer(GLenum target, GLuint bufferID)
{
	assert(target == GL_ARRAY_BUFFER);
}

#endif
