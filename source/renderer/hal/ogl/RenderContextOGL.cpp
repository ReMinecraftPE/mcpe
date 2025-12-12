#include <stdexcept>
#include "RenderContextOGL.hpp"
#include "common/Logger.hpp"
#include "renderer/hal/interface/DepthStencilState.hpp"
#include "renderer/hal/helpers/ErrorHandler.hpp"
#include "world/phys/Vec3.hpp"

using namespace mce;

RenderContextOGL::VertexFieldFormat RenderContextOGL::vertexFieldFormats[] = {
    { GL_FLOAT,          3, GL_FALSE }, // VERTEX_FIELD_POSITION
    { GL_UNSIGNED_BYTE,  4, GL_TRUE  }, // VERTEX_FIELD_COLOR
    { GL_BYTE,           4, GL_FALSE }, // VERTEX_FIELD_NORMAL
#ifdef ENH_GFX_COMPACT_UVS
    { GL_UNSIGNED_SHORT, 2, GL_TRUE  }, // VERTEX_FIELD_UV0
    { GL_UNSIGNED_SHORT, 2, GL_TRUE  }  // VERTEX_FIELD_UV1
#else
    { GL_FLOAT,          2, GL_TRUE  }, // VERTEX_FIELD_UV0
    { GL_FLOAT,          2, GL_TRUE  }  // VERTEX_FIELD_UV1
#endif
};

RenderContextOGL::RenderContextOGL()
    : RenderContextBase()
{
    m_activeTexture = GL_NONE;
    m_activeShaderProgram = GL_NONE;

    m_emptyDepthStencilState = new DepthStencilState();
    mce::DepthStencilStateDescription desc;
    m_emptyDepthStencilState->createDepthState(*(RenderContext*)this, desc);

    clearContextState();
    
    LOG_I("OpenGL Version: %s", gl::getOpenGLVersion().c_str());
}

#ifndef FEATURE_GFX_SHADERS
GLenum _getGLMatrixModeFromMatrixType(MatrixType matrixType)
{
    switch (matrixType)
    {
    case MATRIX_PROJECTION: return GL_PROJECTION;
    case MATRIX_VIEW:       return GL_MODELVIEW;
    default:
        LOG_E("Unknown matrixType: %d", matrixType);
        throw std::bad_cast();
    }
}
#endif

void RenderContextOGL::loadMatrix(MatrixType matrixType, const Matrix& matrix)
{
#ifndef FEATURE_GFX_SHADERS
    GLenum matrixMode = _getGLMatrixModeFromMatrixType(matrixType);
    glMatrixMode(matrixMode);
    glLoadMatrixf(matrix.ptr());
#endif
}

void RenderContextOGL::setVertexState(const VertexFormat& vertexFormat)
{
    RenderContextBase::setVertexState(vertexFormat);

#ifndef FEATURE_GFX_SHADERS
    unsigned int vertexSize = vertexFormat.getVertexSize();

    if (vertexFormat.hasField(VERTEX_FIELD_POSITION))
    {
        const VertexFieldFormat& field = vertexFieldFormats[VERTEX_FIELD_POSITION];
        xglVertexPointer(field.components, field.componentsType, vertexSize, vertexFormat.getFieldOffset(VERTEX_FIELD_POSITION));
        xglEnableClientState(GL_VERTEX_ARRAY);
        ErrorHandler::checkForErrors();
    }

    if (vertexFormat.hasField(VERTEX_FIELD_UV0))
    {
        const VertexFieldFormat& field = vertexFieldFormats[VERTEX_FIELD_UV0];
        xglTexCoordPointer(field.components, field.componentsType, vertexSize, vertexFormat.getFieldOffset(VERTEX_FIELD_UV0));
        xglEnableClientState(GL_TEXTURE_COORD_ARRAY);
        ErrorHandler::checkForErrors();
    }

    if (vertexFormat.hasField(VERTEX_FIELD_COLOR))
    {
        const VertexFieldFormat& field = vertexFieldFormats[VERTEX_FIELD_COLOR];
        xglColorPointer(field.components, field.componentsType, vertexSize, vertexFormat.getFieldOffset(VERTEX_FIELD_COLOR));
        xglEnableClientState(GL_COLOR_ARRAY);
        ErrorHandler::checkForErrors();
    }

#ifdef USE_GL_NORMAL_LIGHTING
    if (vertexFormat.hasField(VERTEX_FIELD_NORMAL))
    {
        const VertexFieldFormat& field = vertexFieldFormats[VERTEX_FIELD_NORMAL];
        xglNormalPointer(field.componentsType, vertexSize, vertexFormat.getFieldOffset(VERTEX_FIELD_NORMAL));
        xglEnableClientState(GL_NORMAL_ARRAY);
        ErrorHandler::checkForErrors();
    }
#endif
#endif
}

void RenderContextOGL::clearVertexState(const VertexFormat& vertexFormat)
{
#ifndef FEATURE_GFX_SHADERS
    if (vertexFormat.hasField(VERTEX_FIELD_POSITION))
        xglDisableClientState(GL_VERTEX_ARRAY);
#ifdef USE_GL_NORMAL_LIGHTING
    if (vertexFormat.hasField(VERTEX_FIELD_NORMAL))
        xglDisableClientState(GL_NORMAL_ARRAY);
#endif
    if (vertexFormat.hasField(VERTEX_FIELD_COLOR))
        xglDisableClientState(GL_COLOR_ARRAY);
    if (vertexFormat.hasField(VERTEX_FIELD_UV0))
        xglDisableClientState(GL_TEXTURE_COORD_ARRAY);
#endif
}

float* _getBuffer(const Vec3& abc, float d)
{
    static float lb[4] = {};

    lb[0] = abc.x;
    lb[1] = abc.y;
    lb[2] = abc.z;
    lb[3] = d;
    return (float*)&lb;
}

void RenderContextOGL::enableFixedLighting(bool init)
{
#ifndef FEATURE_GFX_SHADERS
#ifdef USE_GL_NORMAL_LIGHTING
	glEnable(GL_LIGHTING);
#endif

    if (init)
    {
#ifdef USE_GL_NORMAL_LIGHTING
        glEnable(GL_LIGHT0);
        glEnable(GL_LIGHT1);
#if !defined(__EMSCRIPTEN__) && !defined(USE_GLES)
        glEnable(GL_COLOR_MATERIAL);
        glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
#endif

        constexpr float a = 0.4f, d = 0.6f, s = 0.0f;

        Vec3 l = Vec3(0.2f, 1.0f, -0.7f).normalize();
        glLightfv(GL_LIGHT0, GL_POSITION,      _getBuffer(l,    0.0f));
        glLightfv(GL_LIGHT0, GL_DIFFUSE,       _getBuffer(d,    1.0f));
        glLightfv(GL_LIGHT0, GL_AMBIENT,       _getBuffer(0.0f, 1.0f));
        glLightfv(GL_LIGHT0, GL_SPECULAR,      _getBuffer(s,    1.0f));

        l = Vec3(-0.2f, 1.0f, 0.7f).normalize();
        glLightfv(GL_LIGHT1, GL_POSITION,      _getBuffer(l,    0.0f));
        glLightfv(GL_LIGHT1, GL_DIFFUSE,       _getBuffer(d,    1.0f));
        glLightfv(GL_LIGHT1, GL_AMBIENT,       _getBuffer(0.0f, 1.0f));
        glLightfv(GL_LIGHT1, GL_SPECULAR,      _getBuffer(s,    1.0f));
        glLightModelfv(GL_LIGHT_MODEL_AMBIENT, _getBuffer(a,    1.0f));
#endif
    }
#endif
}

void RenderContextOGL::disableFixedLighting(bool teardown)
{
#ifndef FEATURE_GFX_SHADERS
#ifdef USE_GL_NORMAL_LIGHTING
    glDisable(GL_LIGHTING);
    if (teardown)
    {
        glDisable(GL_LIGHT0);
        glDisable(GL_LIGHT1);
        glDisable(GL_COLOR_MATERIAL);
    }
#endif
#endif
}

bool RenderContextOGL::setShadeMode(ShadeMode mode)
{
    if (!RenderContextBase::setShadeMode(mode))
        return false;

#ifndef FEATURE_GFX_SHADERS
    glShadeModel(shadeModeMap[mode]);
#endif

    return true;
}

bool RenderContextOGL::setCurrentColor(const Color& color)
{
    if (!RenderContextBase::setCurrentColor(color))
        return false;

#ifndef FEATURE_GFX_SHADERS
    glColor4f(color.r, color.g, color.b, color.a);
#endif

    return true;
}

void RenderContextOGL::draw(PrimitiveMode primitiveMode, unsigned int startOffset, unsigned int count)
{
    xglDrawArrays(modeMap[primitiveMode], startOffset, count);
}

void RenderContextOGL::drawIndexed(PrimitiveMode primitiveMode, unsigned int count, uint8_t indexSize)
{
    glDrawElements(modeMap[primitiveMode], count, indexType[indexSize], nullptr);
}

void RenderContextOGL::drawIndexed(PrimitiveMode primitiveMode, unsigned int count, unsigned int startOffset, uint8_t indexSize)
{
    glDrawElements(modeMap[primitiveMode], count, indexType[indexSize], (const GLvoid*)(startOffset * indexSize));
}

void RenderContextOGL::setDepthRange(float nearVal, float farVal)
{
    glDepthRange(nearVal, farVal);
}

void RenderContextOGL::setViewport(int topLeftX, int topLeftY, unsigned int width, unsigned int height, float nearVal, float farVal)
{
    glViewport(topLeftX, topLeftY, width, height);
    setDepthRange(nearVal, farVal);
}

void RenderContextOGL::clearFrameBuffer(const Color& color)
{
    glClearColor(color.r, color.g, color.b, color.a);
    glClear(GL_COLOR_BUFFER_BIT);
}

void RenderContextOGL::clearStencilBuffer()
{
    glStencilMask(0xFFFFFFFF);
    glClear(GL_STENCIL_BUFFER_BIT);
}

void RenderContextOGL::clearDepthStencilBuffer()
{
    // Needed to enable depth write, so we can clear it
    m_emptyDepthStencilState->bindDepthStencilState(*(RenderContext*)this);

    glStencilMask(0xFFFFFFFF);
    glClear(GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

void RenderContextOGL::clearContextState()
{
    m_activeTexture = GL_NONE;
    m_activeBuffer[0] = GL_NONE;
    m_activeBuffer[1] = GL_NONE;

#ifdef MC_GL_DEBUG_OUTPUT
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS_ARB);
    xglDebugMessageCallback(&mce::Platform::OGL::DebugMessage, nullptr);
#endif

#ifdef GL_PERSPECTIVE_CORRECTION_HINT
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);
#endif
    
#ifndef FEATURE_GFX_SHADERS
    disableFixedLighting(false);
#endif
}

void RenderContextOGL::setRenderTarget()
{
}

void RenderContextOGL::swapBuffers()
{
}

int RenderContextOGL::getMaxVertexCount()
{
    return gl::getMaxVertexCount();
}

bool RenderContextOGL::supports32BitIndices()
{
    return gl::supports32BitIndices();
}

GLuint& RenderContextOGL::getActiveBuffer(BufferType bufferType)
{
    if (bufferType < BUFFER_TYPES_MIN || bufferType > BUFFER_TYPES_MAX)
        throw std::out_of_range("m_activeBuffer[]");
    
    return m_activeBuffer[bufferType];
}

GLuint RenderContextOGL::getActiveBuffer(BufferType bufferType) const
{
    if (bufferType < BUFFER_TYPES_MIN || bufferType > BUFFER_TYPES_MAX)
        throw std::out_of_range("m_activeBuffer[]");
    
    return m_activeBuffer[bufferType];
}

RenderContextOGL::ActiveTextureUnit& RenderContextOGL::getActiveTextureUnit(unsigned int index)
{
    if (index >= 8)
        throw std::out_of_range("m_activeTextureUnits[]");
    
    return m_activeTextureUnits[index];
}

const RenderContextOGL::ActiveTextureUnit& RenderContextOGL::getActiveTextureUnit(unsigned int index) const
{
    if (index >= 8)
        throw std::out_of_range("m_activeTextureUnits[]");
    
    return m_activeTextureUnits[index];
}

GLenum mce::getComparisonFunc(ComparisonFunc comparisonFunc)
{
    switch (comparisonFunc)
    {
    case COMPARISON_FUNC_EQUAL:         return GL_EQUAL;
    case COMPARISON_FUNC_NOT_EQUAL:     return GL_NOTEQUAL;
    case COMPARISON_FUNC_ALWAYS:        return GL_ALWAYS;
    case COMPARISON_FUNC_LESS:          return GL_LESS;
    case COMPARISON_FUNC_GREATER:       return GL_GREATER;
    case COMPARISON_FUNC_GREATER_EQUAL: return GL_GEQUAL;
    case COMPARISON_FUNC_LESS_EQUAL:    return GL_LEQUAL;
    default:
        LOG_E("Unknown comparisonFunc: %d", comparisonFunc);
        throw std::bad_cast();
    }
}