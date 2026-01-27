#include "ConstantBufferContainerD3D9.hpp"
#include "renderer/hal/enums/ShaderStagesBits.hpp"
#include "renderer/hal/d3d9/helpers/ErrorHandlerD3D9.hpp"

using namespace mce;

ConstantBufferContainerD3D9::ConstantBufferContainerD3D9()
    : ConstantBufferContainerBase()
{
#if MCE_GFX_D3D9_SHADER_CONSTANT_BUFFERS
	m_vector4fCount = 0;
#endif
}

unsigned int ConstantBufferContainerD3D9::_getVector4fCount() const
{
    if(m_constantBufferBytes->empty())
	{
		LOG_E("m_constantBufferBytes can't be empty");
		throw std::bad_cast();
	}

    // Ceiling division: (A + B - 1) / B
    unsigned int sizeInBytes = m_constantBufferBytes->size();
    unsigned int vectorCount = (sizeInBytes + 15) / 16;
    
    return vectorCount;
}

void ConstantBufferContainerD3D9::sync(RenderContext& context)
{
#if MCE_GFX_D3D9_SHADER_CONSTANT_BUFFERS
    for (size_t i = 0; i < m_shaderConstants->size(); i++)
    {
        const ShaderConstant* shaderConstant = (*m_shaderConstants)[i];
        if (shaderConstant->isDirty())
        {
            void* data = lock(context);
            memcpy(data, &m_constantBufferBytes->front(), m_constantBufferBytes->size());
            unlock(context);
            // it returns in WP 0.12.1, but why?
            return;
        }
    }
#endif
}

void ConstantBufferContainerD3D9::allocateRenderContextBuffer(RenderContext& context)
{
#if MCE_GFX_D3D9_SHADER_CONSTANT_BUFFERS
    m_buffer.release();

	m_vector4fCount = _getVector4fCount();

    D3DDevice d3dDevice = context.getD3DDevice();
    HRESULT hResult = d3dDevice->CreateConstantBuffer(m_vector4fCount, 0x0, *m_buffer);
    ErrorHandlerD3D9::checkForErrors(hResult);
#endif
}

void ConstantBufferContainerD3D9::bindBuffer(RenderContext& context, unsigned int shaderBindPoint, unsigned int shaderStagesBits)
{
#if MCE_GFX_D3D9_SHADER_CONSTANT_BUFFERS
	if (shaderBindPoint + m_vector4fCount > 256)
	{
		LOG_E("Tried to write to constant register beyond 256");
		throw std::bad_cast();
	}

    D3DDevice d3dDevice = context.getD3DDevice();

    if ((shaderStagesBits & SHADER_STAGE_BIT_VERTEX) != 0)
    {
        if (context.m_constantBuffers[SHADER_TYPE_VERTEX][shaderBindPoint] != **m_buffer)
        {
            context.m_constantBuffers[SHADER_TYPE_VERTEX][shaderBindPoint] = **m_buffer;
            d3dDevice->GpuLoadVertexShaderConstantF4(shaderBindPoint, m_vector4fCount, **m_buffer, 0);
        }
    }

    if ((shaderStagesBits & SHADER_STAGE_BIT_PIXEL) != 0)
    {
        if (context.m_constantBuffers[SHADER_TYPE_FRAGMENT][shaderBindPoint] != **m_buffer)
        {
            context.m_constantBuffers[SHADER_TYPE_FRAGMENT][shaderBindPoint] = **m_buffer;
            d3dDevice->GpuLoadPixelShaderConstantF4(shaderBindPoint, m_vector4fCount, **m_buffer, 0);
        }
    }
#endif
}

void* ConstantBufferContainerD3D9::lock(RenderContext& context)
{
    void* pData = nullptr;

#if MCE_GFX_D3D9_SHADER_CONSTANT_BUFFERS
    m_buffer->Lock((D3DVECTOR4**)&pData, 0x0);
#endif

    m_bWriteEnabled = true;

    return pData;
}

void ConstantBufferContainerD3D9::unlock(RenderContext& context)
{
#if MCE_GFX_D3D9_SHADER_CONSTANT_BUFFERS
    m_buffer->Unlock();
#endif

    m_bWriteEnabled = false;
}
