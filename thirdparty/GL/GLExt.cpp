/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#if defined(_WIN32) || defined(__DREAMCAST__)

#ifdef __DREAMCAST__

#define USE_GL_VBO_EMULATION

#endif

#include "GL.hpp"
#include <unordered_map>

#ifdef _WIN32
#ifndef USE_OPENGL_2_FEATURES
// this is stupidly hacky
typedef BOOL(WINAPI* PFNWGLSWAPINTERVALEXTPROC) (int interval);
#endif
#endif

// USE_HARDWARE_GL_BUFFERS enables VBOs to be used using their OpenGL interfaces.
// Disabling it simulates VBO functionality using display lists, which is slower.
#ifndef USE_GL_VBO_EMULATION
#define USE_HARDWARE_GL_BUFFERS
#endif

#ifdef USE_HARDWARE_GL_BUFFERS
PFNGLBINDBUFFERPROC p_glBindBuffer;
PFNGLBUFFERDATAPROC p_glBufferData;
PFNGLGENBUFFERSPROC p_glGenBuffers;
PFNGLDELETEBUFFERSPROC p_glDeleteBuffers;
#endif
#ifndef USE_OPENGL_2_FEATURES
// Note: don't use xglSwapIntervalEXT if you want vsync, you don't know if it's supported
// on your platform so you need to query the extension APIs
PFNWGLSWAPINTERVALEXTPROC p_wglSwapIntervalEXT;
#endif

bool xglInitted()
{
#ifdef USE_HARDWARE_GL_BUFFERS
	return p_glBindBuffer && p_glBufferData && p_glGenBuffers && p_glDeleteBuffers;
#else
	return true;
#endif
}

// Only called on Win32 and SDL+Win32
void xglInit()
{
#ifdef USE_HARDWARE_GL_BUFFERS
#ifdef _WIN32
	p_glBindBuffer = (PFNGLBINDBUFFERPROC)wglGetProcAddress("glBindBuffer");
	p_glBufferData = (PFNGLBUFFERDATAPROC)wglGetProcAddress("glBufferData");
	p_glGenBuffers = (PFNGLGENBUFFERSPROC)wglGetProcAddress("glGenBuffers");
	p_glDeleteBuffers = (PFNGLDELETEBUFFERSPROC)wglGetProcAddress("glDeleteBuffers");
#else
	p_glBindBuffer = (PFNGLBINDBUFFERPROC)glBindBuffer;
	p_glBufferData = (PFNGLBUFFERDATAPROC)glBufferData;
	p_glGenBuffers = (PFNGLGENBUFFERSPROC)glGenBuffers;
	p_glDeleteBuffers = (PFNGLDELETEBUFFERSPROC)glDeleteBuffers;
#endif
#endif

#ifndef USE_OPENGL_2_FEATURES
	p_wglSwapIntervalEXT = (PFNWGLSWAPINTERVALEXTPROC)wglGetProcAddress("wglSwapIntervalEXT");
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

void xglNormalPointer(GLenum type, GLsizei stride, const GLvoid* pointer)
{
#ifdef USE_GL_NORMAL_LIGHTING
	glNormalPointer(type, stride, pointer);
#endif
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

#ifndef xglOrthof

void xglOrthof(GLfloat left, GLfloat right, GLfloat bottom, GLfloat top, GLfloat nearpl, GLfloat farpl)
{
	return glOrtho(GLdouble(left), GLdouble(right), GLdouble(bottom), GLfloat(top), GLdouble(nearpl), GLdouble(farpl));
}

#endif

void xglSwapIntervalEXT(int interval)
{
#ifndef USE_OPENGL_2_FEATURES
	if (!p_wglSwapIntervalEXT)
		return;

	p_wglSwapIntervalEXT(interval);
#endif
}

#ifndef USE_HARDWARE_GL_BUFFERS

// ** Incomplete software based emulation of OpenGL vertex buffers.

#define USE_DISPLAY_LISTS

struct GLBuffer
{
	GLuint  m_id;
	GLvoid* m_pBufferData;
	GLsizei m_bufferSize;
	GLenum  m_usage; // as passed into glBufferData

	// vertex pointer
	GLint   m_vtx_size;
	GLenum  m_vtx_type;
	GLsizei m_vtx_stride;
	GLint   m_vtx_offset;

	// texture coord pointer
	GLint   m_tc_size;
	GLenum  m_tc_type;
	GLsizei m_tc_stride;
	GLint   m_tc_offset;

	// color pointer
	GLint   m_col_size;
	GLenum  m_col_type;
	GLsizei m_col_stride;
	GLint   m_col_offset;

	// normal pointer
	GLenum  m_nor_type;
	GLsizei m_nor_stride;
	GLint   m_nor_offset;

	// associated display list
	GLuint m_AssociatedDisplayList; // used if USE_DISPLAY_LISTS is on

	GLBuffer(GLuint id)
	{
		m_id = id;
		m_pBufferData = nullptr;
		m_bufferSize = 0;
		m_usage = 0;
		m_AssociatedDisplayList = 0;
	}

	void DeletePreExistingDLIfNeeded()
	{
#ifdef USE_DISPLAY_LISTS
		if (m_AssociatedDisplayList != 0)
			glDeleteLists(m_AssociatedDisplayList, 1);

		m_AssociatedDisplayList = 0;
#endif
	}

	bool HasDisplayList()
	{
#ifndef USE_DISPLAY_LISTS
		return false;
#endif
		return m_AssociatedDisplayList != 0;
	}

	GLuint GetDisplayList()
	{
		return m_AssociatedDisplayList;
	}

	void SetDisplayList(GLuint dl)
	{
		m_AssociatedDisplayList = dl;
	}

	void SetVertexPointer(GLint size, GLenum type, GLsizei stride, const GLvoid* offset)
	{
		int ioffset = int(size_t(offset));
		if (m_vtx_size   == size   &&
			m_vtx_type   == type   &&
			m_vtx_stride == stride &&
			m_vtx_offset == ioffset)
			return;
		
		DeletePreExistingDLIfNeeded();
		m_vtx_size = size;
		m_vtx_type = type;
		m_vtx_stride = stride;
		m_vtx_offset = ioffset;
	}

	void SetTextureCoordPointer(GLint size, GLenum type, GLsizei stride, const GLvoid* offset)
	{
		int ioffset = int(size_t(offset));
		if (m_tc_size   == size   &&
			m_tc_type   == type   &&
			m_tc_stride == stride &&
			m_tc_offset == ioffset)
			return;
		
		DeletePreExistingDLIfNeeded();
		m_tc_size = size;
		m_tc_type = type;
		m_tc_stride = stride;
		m_tc_offset = int(size_t(offset));
	}

	void SetColorPointer(GLint size, GLenum type, GLsizei stride, const GLvoid* offset)
	{
		int ioffset = int(size_t(offset));
		if (m_col_size   == size   &&
			m_col_type   == type   &&
			m_col_stride == stride &&
			m_col_offset == ioffset)
			return;
		
		DeletePreExistingDLIfNeeded();
		m_col_size = size;
		m_col_type = type;
		m_col_stride = stride;
		m_col_offset = int(size_t(offset));
	}

	void SetNormalPointer(GLenum type, GLsizei stride, const GLvoid* offset)
	{
		int ioffset = int(size_t(offset));
		if (m_nor_type   == type   &&
			m_nor_stride == stride &&
			m_nor_offset == ioffset)
			return;

		DeletePreExistingDLIfNeeded();
		m_nor_type = type;
		m_nor_stride = stride;
		m_nor_offset = int(size_t(offset));
	}
};

typedef std::unordered_map<GLuint, GLBuffer*> GLBufferMap;

GLBufferMap g_GLBuffers;
GLBuffer* g_pCurrentlyBoundGLBuffer = nullptr;
GLuint g_NextGLBufferID;
bool g_bUseVertexArrays, g_bUseColorArrays, g_bUseNormalArrays, g_bUseTextureCoordArrays; // modified by xgl[En/Dis]ableClientState

void xglGenBuffers(GLsizei num, GLuint* buffers)
{
	for (GLsizei i = 0; i < num; i++)
	{
		*buffers++ = ++g_NextGLBufferID;
		g_GLBuffers[g_NextGLBufferID] = new GLBuffer(g_NextGLBufferID);
	}

	LOG_I("g_NextGLBufferID=%d", g_NextGLBufferID);
}

void xglAssert2(bool condition, const char* condstr, const char* file, int line)
{
	if (condition) return;

	LOG_E("Error: Assertion failed at %s:%d: %s", file, line, condstr);

#ifdef _MSC_VER
	assert(false);
#endif

	exit(1);
}
#define xglAssert(condition) xglAssert2(condition,#condition,__FILE__,__LINE__)

void xglVertexPointer(GLint size, GLenum type, GLsizei stride, const GLvoid* pointer)
{
	xglAssert(g_pCurrentlyBoundGLBuffer != nullptr);
	g_pCurrentlyBoundGLBuffer->SetVertexPointer(size, type, stride, pointer);
}

void xglTexCoordPointer(GLint size, GLenum type, GLsizei stride, const GLvoid* pointer)
{
	xglAssert(g_pCurrentlyBoundGLBuffer != nullptr);
	g_pCurrentlyBoundGLBuffer->SetTextureCoordPointer(size, type, stride, pointer);
}

void xglColorPointer(GLint size, GLenum type, GLsizei stride, const GLvoid* pointer)
{
	xglAssert(g_pCurrentlyBoundGLBuffer != nullptr);
	g_pCurrentlyBoundGLBuffer->SetColorPointer(size, type, stride, pointer);
}

void xglNormalPointer(GLenum type, GLsizei stride, const GLvoid* pointer)
{
	xglAssert(g_pCurrentlyBoundGLBuffer != nullptr);
	g_pCurrentlyBoundGLBuffer->SetNormalPointer(type, stride, pointer);
}

void xglBindBuffer(GLenum target, GLuint bufferID)
{
	xglAssert(target == GL_ARRAY_BUFFER);

	GLBufferMap::iterator iter = g_GLBuffers.find(bufferID);
	if (iter == g_GLBuffers.end())
		return;

	g_pCurrentlyBoundGLBuffer = iter->second;
}

void xglBufferData(GLenum target, GLsizeiptr size, const GLvoid* data, GLenum usage)
{
	xglAssert(target == GL_ARRAY_BUFFER);
	xglAssert(g_pCurrentlyBoundGLBuffer != nullptr);

	GLBuffer* pBuf = g_pCurrentlyBoundGLBuffer;

	// check if the data is the SAME:
	if (pBuf->m_bufferSize == size && memcmp(pBuf->m_pBufferData, data, size) == 0)
	{
		//nope
		pBuf->m_usage = usage;
		return;
	}

	pBuf->DeletePreExistingDLIfNeeded();

	// free the old data, if there was any
	if (pBuf->m_pBufferData)
	{
		free(pBuf->m_pBufferData);
		pBuf->m_pBufferData = nullptr;
	}

	pBuf->m_pBufferData = (GLvoid*)malloc(size);
	xglAssert(pBuf->m_pBufferData != nullptr);

	memcpy(pBuf->m_pBufferData, data, size);

	pBuf->m_usage = usage;
}

void xglDeleteBuffer(GLsizei num)
{
	GLBufferMap::iterator iter = g_GLBuffers.find(num);
	xglAssert(iter != g_GLBuffers.end());

	if (iter->second == g_pCurrentlyBoundGLBuffer)
		g_pCurrentlyBoundGLBuffer = nullptr;

	iter->second->DeletePreExistingDLIfNeeded();

	delete iter->second;
	g_GLBuffers.erase(iter);
}

void xglDeleteBuffers(GLsizei num, GLuint* buffers)
{
	for (GLsizei i = 0; i < num; i++)
	{
		xglDeleteBuffer(buffers[i]);
		buffers[i] = 0;
	}
}

void xglEnableClientState(GLenum _array)
{
	switch (_array)
	{
	case GL_VERTEX_ARRAY:
		g_bUseVertexArrays = true;
		return;
	case GL_COLOR_ARRAY:
		g_bUseColorArrays = true;
		return;
	case GL_NORMAL_ARRAY:
		g_bUseNormalArrays = true;
		return;
	case GL_TEXTURE_COORD_ARRAY:
		g_bUseTextureCoordArrays = true;
		return;
	default:
		glEnableClientState(_array);
	}
}

void xglDisableClientState(GLenum _array)
{
	switch (_array)
	{
	case GL_VERTEX_ARRAY:
		g_bUseVertexArrays = false;
		return;
	case GL_COLOR_ARRAY:
		g_bUseColorArrays = false;
		return;
	case GL_NORMAL_ARRAY:
		g_bUseNormalArrays = false;
		return;
	case GL_TEXTURE_COORD_ARRAY:
		g_bUseTextureCoordArrays = false;
		return;
	default:
		glDisableClientState(_array);
	}
}

void xglDrawArrays(GLenum mode, GLint first, GLsizei count)
{
	xglAssert(g_pCurrentlyBoundGLBuffer != nullptr);
	GLBuffer* pBuf = g_pCurrentlyBoundGLBuffer;

#ifdef USE_DISPLAY_LISTS
	if (pBuf->HasDisplayList())
	{
		glCallList(pBuf->GetDisplayList());
		return;
	}

	GLuint currDL = glGenLists(1);
	pBuf->SetDisplayList(currDL);

	glNewList(currDL, GL_COMPILE);
#endif

	glBegin(mode);

	for (GLsizeiptr i = first, j = 0; j < count; i++, j++)
	{
		uintptr_t addr = uintptr_t(pBuf->m_pBufferData);

		void* pVtx = (void*)(addr + pBuf->m_vtx_offset + i * pBuf->m_vtx_stride);
		void* pCol = (void*)(addr + pBuf->m_col_offset + i * pBuf->m_col_stride);
		void* pNor = (void*)(addr + pBuf->m_col_offset + i * pBuf->m_col_stride);
		void* pTC  = (void*)(addr + pBuf->m_tc_offset  + i * pBuf->m_tc_stride);

		if (g_bUseTextureCoordArrays)
		{
			if (pBuf->m_tc_type == GL_FLOAT)
			{
				float* pfTC = (float*)pTC;
				/**/ if (pBuf->m_tc_size == 2)
					glTexCoord2fv(pfTC);
				else xglAssert(!"Unimplemented texcoord size!");
			}
			else xglAssert(!"Unimplemented texcoord type!");
		}

		if (g_bUseColorArrays)
		{
			if (pBuf->m_col_type == GL_UNSIGNED_BYTE)
			{
				uint8_t* pfCol = (uint8_t*)pCol;
				/**/ if (pBuf->m_col_size == 4)
					glColor4f(float(pfCol[0])/255.0f, float(pfCol[1])/255.0f, float(pfCol[2])/255.0f, float(pfCol[3])/255.0f);
				else xglAssert(!"Unimplemented color size!");
			}
			else xglAssert(!"Unimplemented color type!");
		}

		if (g_bUseNormalArrays)
		{
			//xglAssert(!"Unimplemented normal type!");
		}

		if (g_bUseVertexArrays)
		{
			if (pBuf->m_vtx_type == GL_FLOAT)
			{
				float* pfVtx = (float*)pVtx;
				/**/ if (pBuf->m_vtx_size == 3)
					glVertex3fv(pfVtx);
				else if (pBuf->m_vtx_size == 2)
					glVertex2fv(pfVtx);
				else xglAssert(!"Unimplemented texcoord size!");
			}
			else xglAssert(!"Unimplemented vertex type!");
		}
	}

	glEnd();
#ifdef USE_DISPLAY_LISTS
	glEndList();

	glCallList(pBuf->GetDisplayList());
#endif
}

#endif

#endif
