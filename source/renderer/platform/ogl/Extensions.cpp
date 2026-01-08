#include "Extensions.hpp"
#include "common/Logger.hpp"
#include "GameMods.hpp"

using namespace mce::Platform;

bool OGL::InitBindings()
{
    bool result = true;

#ifdef _WIN32
    xglInit();
    result = xglInitted();
#endif

    return result;
}

void* OGL::GetProcAddress(const char* name)
{
    void* result = nullptr;

#ifdef _WIN32
    result = (void*)wglGetProcAddress(name);
    if (result == nullptr)
    {
        HMODULE handle = GetModuleHandle("opengl32.dll");
        if (handle != NULL)
        {
            result = (void*)GetProcAddress(handle, name);
        }
    }
#endif

    return result;
}

#if defined(_WIN32) || defined(__DREAMCAST__)

#include <unordered_map>

#ifdef __DREAMCAST__

#define USE_GL_VBO_EMULATION

#endif

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
#if GL_VERSION_1_3
PFNGLACTIVETEXTUREPROC p_glActiveTexture;
#endif
#if GL_VERSION_1_5
PFNGLBINDBUFFERPROC p_glBindBuffer;
PFNGLBUFFERDATAPROC p_glBufferData;
PFNGLGENBUFFERSPROC p_glGenBuffers;
PFNGLDELETEBUFFERSPROC p_glDeleteBuffers;
PFNGLBUFFERSUBDATAPROC p_glBufferSubData;
#endif
#if GL_VERSION_2_0
PFNGLSTENCILFUNCSEPARATEPROC p_glStencilFuncSeparate;
PFNGLSTENCILOPSEPARATEPROC p_glStencilOpSeparate;
#endif // GL_VERSION_2_0
#ifdef FEATURE_GFX_SHADERS
#if GL_VERSION_2_0
PFNGLUNIFORM1IPROC p_glUniform1i;
PFNGLUNIFORM1FVPROC p_glUniform1fv;
PFNGLUNIFORM2FVPROC p_glUniform2fv;
PFNGLUNIFORM3FVPROC p_glUniform3fv;
PFNGLUNIFORM4FVPROC p_glUniform4fv;
PFNGLUNIFORM1IVPROC p_glUniform1iv;
PFNGLUNIFORM2IVPROC p_glUniform2iv;
PFNGLUNIFORM3IVPROC p_glUniform3iv;
PFNGLUNIFORM4IVPROC p_glUniform4iv;
PFNGLUNIFORMMATRIX2FVPROC p_glUniformMatrix2fv;
PFNGLUNIFORMMATRIX3FVPROC p_glUniformMatrix3fv;
PFNGLUNIFORMMATRIX4FVPROC p_glUniformMatrix4fv;
PFNGLCREATESHADERPROC p_glCreateShader;
PFNGLSHADERSOURCEPROC p_glShaderSource;
PFNGLCOMPILESHADERPROC p_glCompileShader;
PFNGLGETSHADERIVPROC p_glGetShaderiv;
PFNGLGETSHADERINFOLOGPROC p_glGetShaderInfoLog;
PFNGLDELETESHADERPROC p_glDeleteShader;
PFNGLDELETEPROGRAMPROC p_glDeleteProgram;
PFNGLCREATEPROGRAMPROC p_glCreateProgram;
PFNGLATTACHSHADERPROC p_glAttachShader;
PFNGLLINKPROGRAMPROC p_glLinkProgram;
PFNGLGETPROGRAMIVPROC p_glGetProgramiv;
PFNGLGETPROGRAMINFOLOGPROC p_glGetProgramInfoLog;
PFNGLVERTEXATTRIBPOINTERPROC p_glVertexAttribPointer;
PFNGLUSEPROGRAMPROC p_glUseProgram;
PFNGLGETACTIVEUNIFORMPROC p_glGetActiveUniform;
PFNGLGETACTIVEATTRIBPROC p_glGetActiveAttrib;
PFNGLGETUNIFORMLOCATIONPROC p_glGetUniformLocation;
PFNGLGETATTRIBLOCATIONPROC p_glGetAttribLocation;
PFNGLENABLEVERTEXATTRIBARRAYPROC p_glEnableVertexAttribArray;
#endif // GL_VERSION_2_0
#if GL_VERSION_4_1
PFNGLRELEASESHADERCOMPILERPROC p_glReleaseShaderCompiler;
PFNGLGETSHADERPRECISIONFORMATPROC p_glGetShaderPrecisionFormat;
#endif // GL_VERSION_4_1
#endif // FEATURE_GFX_SHADERS
#ifdef MC_GL_DEBUG_OUTPUT
PFNGLDEBUGMESSAGECALLBACKARBPROC p_glDebugMessageCallback;
#endif
#endif
#ifndef USE_OPENGL_2_FEATURES
// Note: don't use xglSwapIntervalEXT if you want vsync, you don't know if it's supported
// on your platform so you need to query the extension APIs
PFNWGLSWAPINTERVALEXTPROC p_wglSwapIntervalEXT;
#endif

bool xglInitted()
{
#ifdef USE_HARDWARE_GL_BUFFERS
	return p_glActiveTexture
		&& p_glBindBuffer
		&& p_glBufferData
		&& p_glGenBuffers
		&& p_glDeleteBuffers
		&& p_glBufferSubData
#if GL_VERSION_2_0
		&& p_glStencilFuncSeparate
		&& p_glStencilOpSeparate
#ifdef FEATURE_GFX_SHADERS
		&& p_glUniform1i
		&& p_glUniform1fv
		&& p_glUniform2fv
		&& p_glUniform3fv
		&& p_glUniform4fv
		&& p_glUniform1iv
		&& p_glUniform2iv
		&& p_glUniform3iv
		&& p_glUniformMatrix2fv
		&& p_glUniformMatrix3fv
		&& p_glUniformMatrix4fv
		&& p_glCreateShader
		&& p_glShaderSource
		&& p_glCompileShader
		&& p_glGetShaderiv
		&& p_glGetShaderInfoLog
		&& p_glDeleteShader
		&& p_glDeleteProgram
		&& p_glCreateProgram
		&& p_glAttachShader
		&& p_glLinkProgram
		&& p_glGetProgramiv
		&& p_glGetProgramInfoLog
		&& p_glVertexAttribPointer
		&& p_glUseProgram
		&& p_glGetActiveUniform
		&& p_glGetActiveAttrib
		&& p_glGetUniformLocation
		&& p_glGetAttribLocation
		&& p_glEnableVertexAttribArray
#endif // FEATURE_GFX_SHADERS
#endif // GL_VERSION_2_0
#ifdef MC_GL_DEBUG_OUTPUT
		&& p_glDebugMessageCallback
#endif
		;
#else
	return true;
#endif
}

// Only called on Win32 and SDL+Win32
void xglInit()
{
#ifdef USE_HARDWARE_GL_BUFFERS
#ifdef _WIN32
#if GL_VERSION_1_3
	p_glActiveTexture = (PFNGLACTIVETEXTUREPROC)OGL::GetProcAddress("glActiveTexture");
#endif
#if GL_VERSION_1_5
	p_glBindBuffer = (PFNGLBINDBUFFERPROC)OGL::GetProcAddress("glBindBuffer");
	p_glBufferData = (PFNGLBUFFERDATAPROC)OGL::GetProcAddress("glBufferData");
	p_glGenBuffers = (PFNGLGENBUFFERSPROC)OGL::GetProcAddress("glGenBuffers");
	p_glDeleteBuffers = (PFNGLDELETEBUFFERSPROC)OGL::GetProcAddress("glDeleteBuffers");
	p_glBufferSubData = (PFNGLBUFFERSUBDATAPROC)OGL::GetProcAddress("glBufferSubData");
#endif
#if GL_VERSION_2_0
	p_glStencilFuncSeparate = (PFNGLSTENCILFUNCSEPARATEPROC)OGL::GetProcAddress("glStencilFuncSeparate");
	p_glStencilOpSeparate = (PFNGLSTENCILOPSEPARATEPROC)OGL::GetProcAddress("glStencilOpSeparate");
#endif
#ifdef FEATURE_GFX_SHADERS
#if GL_VERSION_2_0
	p_glUniform1i = (PFNGLUNIFORM1IPROC)OGL::GetProcAddress("glUniform1i");
	p_glUniform1fv = (PFNGLUNIFORM1FVPROC)OGL::GetProcAddress("glUniform1fv");
	p_glUniform2fv = (PFNGLUNIFORM2FVPROC)OGL::GetProcAddress("glUniform2fv");
	p_glUniform3fv = (PFNGLUNIFORM3FVPROC)OGL::GetProcAddress("glUniform3fv");
	p_glUniform4fv = (PFNGLUNIFORM4FVPROC)OGL::GetProcAddress("glUniform4fv");
	p_glUniform1iv = (PFNGLUNIFORM1IVPROC)OGL::GetProcAddress("glUniform1iv");
	p_glUniform2iv = (PFNGLUNIFORM2IVPROC)OGL::GetProcAddress("glUniform2iv");
	p_glUniform3iv = (PFNGLUNIFORM3IVPROC)OGL::GetProcAddress("glUniform3iv");
	p_glUniform4iv = (PFNGLUNIFORM4IVPROC)OGL::GetProcAddress("glUniform4iv");
	p_glUniformMatrix2fv = (PFNGLUNIFORMMATRIX2FVPROC)OGL::GetProcAddress("glUniformMatrix2fv");
	p_glUniformMatrix3fv = (PFNGLUNIFORMMATRIX3FVPROC)OGL::GetProcAddress("glUniformMatrix3fv");
	p_glUniformMatrix4fv = (PFNGLUNIFORMMATRIX4FVPROC)OGL::GetProcAddress("glUniformMatrix4fv");
	p_glCreateShader = (PFNGLCREATESHADERPROC)OGL::GetProcAddress("glCreateShader");
	p_glShaderSource = (PFNGLSHADERSOURCEPROC)OGL::GetProcAddress("glShaderSource");
	p_glCompileShader = (PFNGLCOMPILESHADERPROC)OGL::GetProcAddress("glCompileShader");
	p_glGetShaderiv = (PFNGLGETSHADERIVPROC)OGL::GetProcAddress("glGetShaderiv");
	p_glGetShaderInfoLog = (PFNGLGETSHADERINFOLOGPROC)OGL::GetProcAddress("glGetShaderInfoLog");
	p_glDeleteShader = (PFNGLDELETESHADERPROC)OGL::GetProcAddress("glDeleteShader");
	p_glDeleteProgram = (PFNGLDELETEPROGRAMPROC)OGL::GetProcAddress("glDeleteProgram");
	p_glCreateProgram = (PFNGLCREATEPROGRAMPROC)OGL::GetProcAddress("glCreateProgram");
	p_glAttachShader = (PFNGLATTACHSHADERPROC)OGL::GetProcAddress("glAttachShader");
	p_glLinkProgram = (PFNGLLINKPROGRAMPROC)OGL::GetProcAddress("glLinkProgram");
	p_glGetProgramiv = (PFNGLGETPROGRAMIVPROC)OGL::GetProcAddress("glGetProgramiv");
	p_glGetProgramInfoLog = (PFNGLGETPROGRAMINFOLOGPROC)OGL::GetProcAddress("glGetProgramInfoLog");
	p_glVertexAttribPointer = (PFNGLVERTEXATTRIBPOINTERPROC)OGL::GetProcAddress("glVertexAttribPointer");
	p_glUseProgram = (PFNGLUSEPROGRAMPROC)OGL::GetProcAddress("glUseProgram");
	p_glGetActiveUniform = (PFNGLGETACTIVEUNIFORMPROC)OGL::GetProcAddress("glGetActiveUniform");
	p_glGetActiveAttrib = (PFNGLGETACTIVEATTRIBPROC)OGL::GetProcAddress("glGetActiveAttrib");
	p_glGetUniformLocation = (PFNGLGETUNIFORMLOCATIONPROC)OGL::GetProcAddress("glGetUniformLocation");
	p_glGetAttribLocation = (PFNGLGETATTRIBLOCATIONPROC)OGL::GetProcAddress("glGetAttribLocation");
	p_glEnableVertexAttribArray = (PFNGLENABLEVERTEXATTRIBARRAYPROC)OGL::GetProcAddress("glEnableVertexAttribArray");
#endif // GL_VERSION_2_0
#if GL_VERSION_4_1
	p_glReleaseShaderCompiler = (PFNGLRELEASESHADERCOMPILERPROC)OGL::GetProcAddress("glReleaseShaderCompiler");
	p_glGetShaderPrecisionFormat = (PFNGLGETSHADERPRECISIONFORMATPROC)OGL::GetProcAddress("glGetShaderPrecisionFormat");
#endif // GL_VERSION_4_1
#endif // FEATURE_GFX_SHADERS
#ifdef MC_GL_DEBUG_OUTPUT
	p_glDebugMessageCallback = (PFNGLDEBUGMESSAGECALLBACKARBPROC)OGL::GetProcAddress("glDebugMessageCallback");
#endif
#else // !defined(_WIN32) // wtf would this even be?
#if GL_VERSION_1_3
	p_glActiveTexture = (PFNGLACTIVETEXTUREPROC)glActiveTexture;
	p_glLoadTransposeMatrixf = (PFNGLLOADTRANSPOSEMATRIXFPROC)glLoadTransposeMatrixf;
#endif
#if GL_VERSION_1_5
	p_glBindBuffer = (PFNGLBINDBUFFERPROC)glBindBuffer;
	p_glBufferData = (PFNGLBUFFERDATAPROC)glBufferData;
	p_glGenBuffers = (PFNGLGENBUFFERSPROC)glGenBuffers;
	p_glDeleteBuffers = (PFNGLDELETEBUFFERSPROC)glDeleteBuffers;
	p_glBufferSubData = (PFNGLBUFFERSUBDATAPROC)glBufferSubData;
#endif
#if GL_VERSION_2_0
	p_glStencilFuncSeparate = (PFNGLSTENCILFUNCSEPARATEPROC)glStencilFuncSeparate;
	p_glStencilOpSeparate = (PFNGLSTENCILOPSEPARATEPROC)glStencilOpSeparate;
#endif
#ifdef FEATURE_GFX_SHADERS
#if GL_VERSION_2_0
	p_glUniform1i = (PFNGLUNIFORM1IPROC)glUniform1i;
	p_glUniform1fv = (PFNGLUNIFORM1FVPROC)glUniform1fv;
	p_glUniform2fv = (PFNGLUNIFORM2FVPROC)glUniform2fv;
	p_glUniform3fv = (PFNGLUNIFORM3FVPROC)glUniform3fv;
	p_glUniform4fv = (PFNGLUNIFORM4FVPROC)glUniform4fv;
	p_glUniform1iv = (PFNGLUNIFORM1IVPROC)glUniform1iv;
	p_glUniform2iv = (PFNGLUNIFORM2IVPROC)glUniform2iv;
	p_glUniform3iv = (PFNGLUNIFORM3IVPROC)glUniform3iv;
	p_glUniform4iv = (PFNGLUNIFORM4IVPROC)glUniform4iv;
	p_glUniformMatrix2fv = (PFNGLUNIFORMMATRIX2FVPROC)glUniformMatrix2fv;
	p_glUniformMatrix3fv = (PFNGLUNIFORMMATRIX3FVPROC)glUniformMatrix3fv;
	p_glUniformMatrix4fv = (PFNGLUNIFORMMATRIX4FVPROC)glUniformMatrix4fv;
	p_glCreateShader = (PFNGLCREATESHADERPROC)glCreateShader;
	p_glShaderSource = (PFNGLSHADERSOURCEPROC)glShaderSource;
	p_glCompileShader = (PFNGLCOMPILESHADERPROC)glCompileShader;
	p_glGetShaderiv = (PFNGLGETSHADERIVPROC)glGetShaderiv;
	p_glGetShaderInfoLog = (PFNGLGETSHADERINFOLOGPROC)glGetShaderInfoLog;
	p_glDeleteShader = (PFNGLDELETESHADERPROC)glDeleteShader;
	p_glDeleteProgram = (PFNGLDELETEPROGRAMPROC)glDeleteProgram;
	p_glCreateProgram = (PFNGLCREATEPROGRAMPROC)glCreateProgram;
	p_glAttachShader = (PFNGLATTACHSHADERPROC)glAttachShader;
	p_glLinkProgram = (PFNGLLINKPROGRAMPROC)glLinkProgram;
	p_glGetProgramiv = (PFNGLGETPROGRAMIVPROC)glGetProgramiv;
	p_glGetProgramInfoLog = (PFNGLGETPROGRAMINFOLOGPROC)glGetProgramInfoLog;
	p_glVertexAttribPointer = (PFNGLVERTEXATTRIBPOINTERPROC)glVertexAttribPointer;
	p_glUseProgram = (PFNGLUSEPROGRAMPROC)glUseProgram;
	p_glGetActiveUniform = (PFNGLGETACTIVEUNIFORMPROC)glGetActiveUniform;
	p_glGetActiveAttrib = (PFNGLGETACTIVEATTRIBPROC)glGetActiveAttrib;
	p_glGetUniformLocation = (PFNGLGETUNIFORMLOCATIONPROC)glGetUniformLocation;
	p_glGetAttribLocation = (PFNGLGETATTRIBLOCATIONPROC)glGetAttribLocation;
	p_glEnableVertexAttribArray = (PFNGLENABLEVERTEXATTRIBARRAYPROC)glEnableVertexAttribArray;
#endif // GL_VERSION_2_0
#if GL_VERSION_4_1
	p_glReleaseShaderCompiler = (PFNGLRELEASESHADERCOMPILERPROC)glReleaseShaderCompiler;
	p_glGetShaderPrecisionFormat = (PFNGLGETSHADERPRECISIONFORMATPROC)glGetShaderPrecisionFormat;
#endif // GL_VERSION_4_1
#endif // FEATURE_GFX_SHADERS
#ifdef MC_GL_DEBUG_OUTPUT
	p_glDebugMessageCallback = (PFNGLDEBUGMESSAGECALLBACKARBPROC)glDebugMessageCallbackARB;
#endif
#endif
#endif

#ifndef USE_OPENGL_2_FEATURES
	p_wglSwapIntervalEXT = (PFNWGLSWAPINTERVALEXTPROC)OGL::GetProcAddress("wglSwapIntervalEXT");
#endif
}

#ifdef USE_HARDWARE_GL_BUFFERS

#if GL_VERSION_1_3

void xglActiveTexture(GLenum texture)
{
	p_glActiveTexture(texture);
}

#endif // GL_VERSION_1_3

#if GL_VERSION_1_5

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

void xglBufferSubData(GLenum target, GLintptr offset, GLsizeiptr size, const GLvoid* data)
{
	p_glBufferSubData(target, offset, size, data);
}

#endif // GL_VERSION_1_5

#if GL_VERSION_2_0

void xglStencilFuncSeparate(GLenum face, GLenum func, GLint ref, GLuint mask)
{
	p_glStencilFuncSeparate(face, func, ref, mask);
}

void xglStencilOpSeparate(GLenum face, GLenum sfail, GLenum dpfail, GLenum dppass)
{
	p_glStencilOpSeparate(face, sfail, dpfail, dppass);
}

#endif // GL_VERSION_2_0

#ifdef FEATURE_GFX_SHADERS

#if GL_VERSION_2_0

void xglUniform1i(GLint location, GLint v0)
{
	p_glUniform1i(location, v0);
}

void xglUniform1fv(GLint location, GLsizei count, const GLfloat* value)
{
	p_glUniform1fv(location, count, value);
}

void xglUniform2fv(GLint location, GLsizei count, const GLfloat* value)
{
	p_glUniform2fv(location, count, value);
}

void xglUniform3fv(GLint location, GLsizei count, const GLfloat* value)
{
	p_glUniform3fv(location, count, value);
}

void xglUniform4fv(GLint location, GLsizei count, const GLfloat* value)
{
	p_glUniform4fv(location, count, value);
}

void xglUniform1iv(GLint location, GLsizei count, const GLint* value)
{
	p_glUniform1iv(location, count, value);
}

void xglUniform2iv(GLint location, GLsizei count, const GLint* value)
{
	p_glUniform2iv(location, count, value);
}

void xglUniform3iv(GLint location, GLsizei count, const GLint* value)
{
	p_glUniform3iv(location, count, value);
}

void xglUniform4iv(GLint location, GLsizei count, const GLint* value)
{
	p_glUniform4iv(location, count, value);
}

void xglUniformMatrix2fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value)
{
	p_glUniformMatrix2fv(location, count, transpose, value);
}

void xglUniformMatrix3fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value)
{
	p_glUniformMatrix3fv(location, count, transpose, value);
}

void xglUniformMatrix4fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value)
{
	p_glUniformMatrix4fv(location, count, transpose, value);
}

GLuint xglCreateShader(GLenum type)
{
	return p_glCreateShader(type);
}

void xglShaderSource(GLuint shader, GLsizei count, const GLchar** string, const GLint* length)
{
	p_glShaderSource(shader, count, string, length);
}

void xglCompileShader(GLuint shader)
{
	p_glCompileShader(shader);
}

void xglGetShaderiv(GLuint shader, GLenum pname, GLint* params)
{
	p_glGetShaderiv(shader, pname, params);
}

void xglGetShaderInfoLog(GLuint shader, GLsizei bufSize, GLsizei* length, GLchar* infoLog)
{
	p_glGetShaderInfoLog(shader, bufSize, length, infoLog);
}

void xglDeleteShader(GLuint shader)
{
	p_glDeleteShader(shader);
}

void xglDeleteProgram(GLuint program)
{
	p_glDeleteProgram(program);
}

GLuint xglCreateProgram()
{
	return p_glCreateProgram();
}

void xglAttachShader(GLuint program, GLuint shader)
{
	p_glAttachShader(program, shader);
}

void xglLinkProgram(GLuint program)
{
	p_glLinkProgram(program);
}

void xglGetProgramiv(GLuint program, GLenum pname, GLint* params)
{
	p_glGetProgramiv(program, pname, params);
}

void xglGetProgramInfoLog(GLuint program, GLsizei bufSize, GLsizei* length, GLchar* infoLog)
{
	p_glGetProgramInfoLog(program, bufSize, length, infoLog);
}

void xglVertexAttribPointer(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid* pointer)
{
	p_glVertexAttribPointer(index, size, type, normalized, stride, pointer);
}

void xglUseProgram(GLuint program)
{
	p_glUseProgram(program);
}

void xglGetActiveUniform(GLuint program, GLuint index, GLsizei bufSize, GLsizei* length, GLint* size, GLenum* type, GLchar* name)
{
	p_glGetActiveUniform(program, index, bufSize, length, size, type, name);
}

void xglGetActiveAttrib(GLuint program, GLuint index, GLsizei bufSize, GLsizei* length, GLint* size, GLenum* type, GLchar* name)
{
	p_glGetActiveAttrib(program, index, bufSize, length, size, type, name);
}

GLint xglGetUniformLocation(GLuint program, const GLchar* name)
{
	return p_glGetUniformLocation(program, name);
}

GLint xglGetAttribLocation(GLuint program, const GLchar* name)
{
	return p_glGetAttribLocation(program, name);
}

void xglEnableVertexAttribArray(GLuint index)
{
	p_glEnableVertexAttribArray(index);
}

#endif // GL_VERSION_2_0

#if GL_VERSION_4_1

void xglReleaseShaderCompiler()
{
	if (!p_glReleaseShaderCompiler)
		return;

	p_glReleaseShaderCompiler();
}

void xglGetShaderPrecisionFormat(GLenum shadertype, GLenum precisiontype, GLint* range, GLint* precision)
{
	if (!p_glGetShaderPrecisionFormat)
		return;
	
	p_glGetShaderPrecisionFormat(shadertype, precisiontype, range, precision);
}

#endif // GL_VERSION_4_1

#endif // FEATURE_GFX_SHADERS

#ifdef MC_GL_DEBUG_OUTPUT
void xglDebugMessageCallback(DEBUGPROC callback, GLvoid* userParam)
{
	p_glDebugMessageCallback(callback, userParam);
}
#endif

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
