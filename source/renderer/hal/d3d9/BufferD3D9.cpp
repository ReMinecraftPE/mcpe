#include <typeinfo>
#include "BufferD3D9.hpp"
#include "common/Logger.hpp"
#include "renderer/hal/d3d9/helpers/ErrorHandlerD3D9.hpp"
#include "renderer/hal/interface/RenderContext.hpp"

using namespace mce;

D3DFORMAT D3DFormatFromStride(unsigned int stride)
{
    switch (stride)
    {
    case 1:  return D3DFMT_UNKNOWN;
    case 2:  return D3DFMT_INDEX16;
    case 4:  return D3DFMT_INDEX32;
    default:
        LOG_E("Unsupported stride: %d", stride);
        throw std::bad_cast();
    }
}

DWORD mapTypeToD3DLockType(MapType mapType)
{
    switch (mapType)
    {
    case MAP_READ:               return D3DLOCK_READONLY;
    case MAP_WRITE:              return 0x0;
    case MAP_WRITE_DISCARD:
#ifdef D3DLOCK_DISCARD
		return D3DLOCK_DISCARD;
#else
		return 0x0;
#endif
    case MAP_WRITE_NO_OVERWRITE: return D3DLOCK_NOOVERWRITE;
    default:
        LOG_E("Unknown mapType: %d", mapType);
        throw std::bad_cast();
    }
}

BufferD3D9::BufferD3D9()
{
    m_format = D3DFMT_UNKNOWN;
    m_offset = 0;
}

BufferD3D9::~BufferD3D9()
{
    releaseBuffer();
}

void BufferD3D9::_createBuffer(RenderContext& context, unsigned int stride, const void* data, unsigned int count, BufferType bufferType, bool isDynamic)
{
    D3DDevice d3dDevice = context.getD3DDevice();
    DWORD usage = isDynamic ? D3DUSAGE_DYNAMIC : 0x0;
    unsigned int size = stride * count;

    m_vertexBuffer.release();
    m_indexBuffer.release();

    switch (bufferType)
    {
        case BUFFER_TYPE_VERTEX:
        {
            ErrorHandlerD3D9::checkForErrors(
                d3dDevice->CreateVertexBuffer(
                    size,
                    usage,
                    0, D3DPOOL_DEFAULT,
                    *m_vertexBuffer,
                    NULL
                )
            );
            break;
        }
        case BUFFER_TYPE_INDEX:
        {
            m_format = D3DFormatFromStride(stride);
            ErrorHandlerD3D9::checkForErrors(
                d3dDevice->CreateIndexBuffer(
                    size,
                    usage,
                    m_format,
                    D3DPOOL_DEFAULT,
                    *m_indexBuffer,
                    NULL
                )
            );
            break;
        }
        default:
        {
            LOG_E("Unknown bufferType: %d", bufferType);
            throw std::bad_cast();
        }
    }

    if (data)
    {
        void* pData = (void*)data;
        updateBuffer(context, stride, pData, count);
    }
}

void BufferD3D9::_move(BufferD3D9& other)
{
    if (this != &other)
    {
        this->releaseBuffer();
		
        this->m_vertexBuffer._move(other.m_vertexBuffer);
        this->m_indexBuffer._move(other.m_indexBuffer);
        std::swap(this->m_format, other.m_format);
        std::swap(this->m_offset, other.m_offset);
    }
	
    BufferBase::_move(other);
}

void BufferD3D9::releaseBuffer()
{
    m_vertexBuffer.release();
    m_indexBuffer.release();

    BufferBase::releaseBuffer();
}

void BufferD3D9::bindBuffer(RenderContext& context)
{
    D3DDevice d3dDevice = context.getD3DDevice();

    switch (m_bufferType)
    {
    case BUFFER_TYPE_VERTEX:
        d3dDevice->SetStreamSource(0, **m_vertexBuffer, m_offset, m_stride);
        break;
    case BUFFER_TYPE_INDEX:
        d3dDevice->SetIndices(**m_indexBuffer);
        break;
    default:
        LOG_E("Unknown m_bufferType: %d", m_bufferType);
        throw std::bad_cast();
    }
}

void BufferD3D9::createBuffer(RenderContext& context, unsigned int stride, const void *data, unsigned int count, BufferType bufferType)
{
    BufferBase::createBuffer(context, stride, data, count, bufferType);
    _createBuffer(context, stride, data, count, bufferType, false);
}

void BufferD3D9::createDynamicBuffer(RenderContext& context, unsigned int stride, const void* data, unsigned int count, BufferType bufferType)
{
    BufferBase::createDynamicBuffer(context, stride, data, count, bufferType);
    _createBuffer(context, stride, data, count, bufferType, true);
}

void BufferD3D9::resizeBuffer(RenderContext& context, const void* data, unsigned int size)
{
    LOG_E("BufferD3D9::resizeBuffer() not implemented");
    throw std::bad_cast();
}

void BufferD3D9::updateBuffer(RenderContext& context, unsigned int stride, void*& data, unsigned int count, MapType mapType)
{
    if (m_internalSize < stride * count)
    {
        createDynamicBuffer(context, stride * count, data, count, m_bufferType);
        return;
    }

    void* pData;
    DWORD lockFlags = mapTypeToD3DLockType(mapType);

    switch (m_bufferType)
    {
    case BUFFER_TYPE_VERTEX:
        m_vertexBuffer->Lock(0, stride * count, &pData, lockFlags);
        break;
    case BUFFER_TYPE_INDEX:
        m_format = D3DFormatFromStride(stride);
        m_indexBuffer->Lock(0, stride * count, &pData, lockFlags);
        break;
    default:
        LOG_E("Unknown bufferType: %d", m_bufferType);
        throw std::bad_cast();
    }

    memcpy((int8_t*)pData + m_offset, data, stride * count);
 
    switch (m_bufferType)
    {
    case BUFFER_TYPE_VERTEX:
        m_vertexBuffer->Unlock();
        break;
    case BUFFER_TYPE_INDEX:
        m_indexBuffer->Unlock();
        break;
    }
        
    BufferBase::updateBuffer(context, stride, data, count);
}