#include <typeinfo>
#include "BufferOGL.hpp"
#include "common/Logger.hpp"
#include "renderer/hal/helpers/ErrorHandler.hpp"
#include "renderer/hal/interface/RenderContext.hpp"

using namespace mce;

GLenum mce::glTargetFromBufferType(BufferType bufferType)
{
    switch (bufferType)
    {
        case BUFFER_TYPE_VERTEX:
            return GL_ARRAY_BUFFER;
        case BUFFER_TYPE_INDEX:
            return GL_ELEMENT_ARRAY_BUFFER;
        default:
            LOG_E("Unknown bufferType: %d", bufferType);
            throw std::bad_cast();
    }
}

BufferOGL::BufferOGL()
{
    m_bufferName = GL_NONE;
    m_target = GL_NONE;
    m_usage = GL_STATIC_DRAW;
}

BufferOGL::~BufferOGL()
{
    releaseBuffer();
}

void BufferOGL::_createBuffer(RenderContext& context, unsigned int stride, const void* data, unsigned int count, BufferType bufferType)
{
    ErrorHandler::checkForErrors();

    m_target = mce::glTargetFromBufferType(bufferType);

    xglGenBuffers(1, &m_bufferName);
    xglBindBuffer(m_target, m_bufferName);

    // Set active buffer
    GLuint& activeBuffer = context.getActiveBuffer(m_bufferType);
    activeBuffer = m_bufferName;

    xglBufferData(m_target, m_internalSize, data, m_usage);

    ErrorHandler::checkForErrors();
}

void BufferOGL::_move(BufferOGL& other)
{
    if (this != &other)
    {
        this->releaseBuffer();
		
        this->m_target = other.m_target;
        this->m_bufferName = other.m_bufferName;
        this->m_usage = other.m_usage;
        
        other.m_bufferName = GL_NONE;
        other.m_target = GL_NONE;
        other.m_usage = GL_NONE;
    }
	
    BufferBase::_move(other);
}

void BufferOGL::releaseBuffer()
{
    if (isValid())
        xglDeleteBuffers(1, &m_bufferName);

    m_bufferName = GL_NONE;
    m_target = GL_NONE;

    BufferBase::releaseBuffer();
}

void BufferOGL::bindBuffer(RenderContext& context)
{
    GLuint& activeBuffer = context.getActiveBuffer(m_bufferType);
    if (activeBuffer == m_bufferName)
        return;
    
    xglBindBuffer(m_target, m_bufferName);
    activeBuffer = m_bufferName;
}

void BufferOGL::createBuffer(RenderContext& context, unsigned int stride, const void *data, unsigned int count, BufferType bufferType)
{
    BufferBase::createBuffer(context, stride, data, count, bufferType);
    
    m_usage = GL_STATIC_DRAW;
    _createBuffer(context, stride, data, count, bufferType);
}

void BufferOGL::createDynamicBuffer(RenderContext& context, unsigned int stride, const void* data, unsigned int count, BufferType bufferType)
{
    BufferBase::createDynamicBuffer(context, stride, data, count, bufferType);

    // Mojang used GL_STREAM_DRAW in 0.16.1, and GL_STATIC_DRAW in 0.12.1
#ifdef GL_STREAM_DRAW
    m_usage = GL_STREAM_DRAW;
#else // GLES 1
    m_usage = GL_DYNAMIC_DRAW;
#endif
    _createBuffer(context, stride, data, count, bufferType);
}

void BufferOGL::resizeBuffer(RenderContext& context, const void* data, unsigned int size)
{
    xglBufferData(m_target, size, data, m_usage);
    m_internalSize = size;
}

void BufferOGL::updateBuffer(RenderContext& context, unsigned int stride, void*& data, unsigned int count)
{
    bindBuffer(context);

    // https://community.khronos.org/t/vbo-test-glbufferdata-vs-glbuffersubdata-vs-glmapbufferoes/2748
    // Summary: Calling glBufferData is significantly faster than calling glBufferSubData
    // because whoever did the GLES 1 implementation at Apple is retarded.
    // This may be fixed by acquiring a GLES 2 context, but doing that causes nothing to
    // to be rendered, so perhaps some day...
    // Additionally, we could try holding the vertex buffer data in memory and pass
    // it in the draw call, as supposedly not even using buffers is faster.
    bool useAppleWorkaround = false;
#if MC_PLATFORM_IOS
    useAppleWorkaround = true;
#endif
    
    const unsigned int size = count * stride;

    if (!useAppleWorkaround && size <= m_internalSize)
        xglBufferSubData(m_target, m_bufferOffset, size, data);
    else
        resizeBuffer(context, data, size);
        
    BufferBase::updateBuffer(context, stride, data, count);
}