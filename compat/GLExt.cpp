/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "GL.hpp"

HWND GetHWND();
extern LPCTSTR g_GameTitle;

// this sucks... F* you Microsoft for making me do this hacky ass bullsh*t.

PFNGLBINDBUFFERPROC p_glBindBuffer;
PFNGLBUFFERDATAPROC p_glBufferData;
PFNGLGENBUFFERSPROC p_glGenBuffers;
PFNGLDELETEBUFFERSPROC p_glDeleteBuffers;

// this is stupidly hacky
typedef BOOL(WINAPI* PFNWGLSWAPINTERVALEXTPROC) (int interval);

PFNWGLSWAPINTERVALEXTPROC p_wglSwapIntervalEXT;

bool xglInitted()
{
	return p_glBindBuffer && p_glBufferData && p_glGenBuffers && p_glDeleteBuffers;
}

void xglInit()
{
	p_glBindBuffer = (PFNGLBINDBUFFERPROC)wglGetProcAddress("glBindBuffer");
	p_glBufferData = (PFNGLBUFFERDATAPROC)wglGetProcAddress("glBufferData");
	p_glGenBuffers = (PFNGLGENBUFFERSPROC)wglGetProcAddress("glGenBuffers");
	p_glDeleteBuffers = (PFNGLDELETEBUFFERSPROC)wglGetProcAddress("glDeleteBuffers");

	p_wglSwapIntervalEXT = (PFNWGLSWAPINTERVALEXTPROC)wglGetProcAddress("wglSwapIntervalEXT");

	if (!xglInitted())
	{
		MessageBox(GetHWND(), TEXT("Error initializing GL extensions. Update your graphics drivers!"), g_GameTitle, MB_OK);
	}
}

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
