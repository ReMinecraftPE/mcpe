#include "ConstantBufferContainerD3D11.hpp"
#include "renderer/hal/enums/ShaderStagesBits.hpp"
#include "renderer/hal/dxgi/helpers/ErrorHandlerDXGI.hpp"

using namespace mce;

ConstantBufferContainerD3D11::ConstantBufferContainerD3D11()
    : ConstantBufferContainerBase()
{
}

void ConstantBufferContainerD3D11::sync(RenderContext& context)
{
    for (int i = 0; i < m_shaderConstants->size(); i++)
    {
        const ShaderConstant* shaderConstant = (*m_shaderConstants)[i];
        if (shaderConstant->isDirty())
        {
            void* data = map(context);
            memcpy(data, &m_constantBufferBytes->front(), m_constantBufferBytes->size());
            unmap(context);
            // it returns in WP 0.12.1, but why?
            return;
        }
    }
}

void ConstantBufferContainerD3D11::allocateRenderContextBuffer(RenderContext& context)
{
    CD3D11_BUFFER_DESC desc;
    {
        desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        desc.ByteWidth = m_constantBufferBytes->size();
        desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        desc.Usage = D3D11_USAGE_DYNAMIC;
        desc.MiscFlags = 0x0;
        desc.StructureByteStride = 0;
    }

    m_buffer.release();
    D3DDevice d3dDevice = context.getD3DDevice();
    HRESULT hResult = d3dDevice->CreateBuffer(&desc, NULL, *m_buffer);
    ErrorHandlerDXGI::checkForErrors(hResult);
}

void ConstantBufferContainerD3D11::bindBuffer(RenderContext& context, unsigned int shaderBindPoint, unsigned int shaderStagesBits)
{
    D3DDeviceContext d3dDeviceContext = context.getD3DDeviceContext();

    if ((shaderStagesBits & SHADER_STAGE_BIT_VERTEX) != 0)
    {
        if (context.m_constantBuffers[SHADER_TYPE_VERTEX][shaderBindPoint] != **m_buffer)
        {
            context.m_constantBuffers[SHADER_TYPE_VERTEX][shaderBindPoint] = **m_buffer;
            d3dDeviceContext->VSSetConstantBuffers(shaderBindPoint, 1, *m_buffer);
        }
    }

    if ((shaderStagesBits & SHADER_STAGE_BIT_PIXEL) != 0)
    {
        if (context.m_constantBuffers[SHADER_TYPE_FRAGMENT][shaderBindPoint] != **m_buffer)
        {
            context.m_constantBuffers[SHADER_TYPE_FRAGMENT][shaderBindPoint] = **m_buffer;
            d3dDeviceContext->PSSetConstantBuffers(shaderBindPoint, 1, *m_buffer);
        }
    }

    if ((shaderStagesBits & SHADER_STAGE_BIT_GEOMETRY) != 0)
    {
        if (context.m_constantBuffers[SHADER_TYPE_GEOMETRY][shaderBindPoint] != **m_buffer)
        {
            context.m_constantBuffers[SHADER_TYPE_GEOMETRY][shaderBindPoint] = **m_buffer;
            d3dDeviceContext->GSSetConstantBuffers(shaderBindPoint, 1, *m_buffer);
        }
    }
}

void* ConstantBufferContainerD3D11::map(RenderContext& context)
{
    D3D11_MAPPED_SUBRESOURCE subResource = {0};

    D3DDeviceContext d3dDeviceContext = context.getD3DDeviceContext();
    HRESULT hResult = d3dDeviceContext->Map(**m_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0x0, &subResource);
    ErrorHandlerDXGI::checkForErrors(hResult);

    m_bCurrentlyMapped = true;

    return subResource.pData;
}

void ConstantBufferContainerD3D11::unmap(RenderContext& context)
{
    D3DDeviceContext d3dDeviceContext = context.getD3DDeviceContext();
    d3dDeviceContext->Unmap(**m_buffer, 0);

    m_bCurrentlyMapped = true; // yeah, they did this
}