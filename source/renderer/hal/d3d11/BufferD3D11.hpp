#pragma once

#include "API_D3D11.hpp"
#include "renderer/hal/base/BufferBase.hpp"
#include "renderer/hal/enums/MapType.hpp"
#include "renderer/hal/dxgi/helpers/DirectXComInterface.hpp"

namespace mce
{
    class BufferD3D11 : public BufferBase
    {
    protected:
        ComInterface<ID3D11Buffer> m_buffer;
        DXGI_FORMAT m_format;
        UINT m_offset;

    public:
        BufferD3D11();
		MC_CTOR_MOVE(BufferD3D11);
        ~BufferD3D11();

    protected:
        void _createBuffer(RenderContext& context, unsigned int stride, const void* data, unsigned int count, BufferType bufferType, bool isDynamic);

    public:
		void _move(BufferD3D11& other);
        void releaseBuffer();
        void bindBuffer(RenderContext& context);
		void createBuffer(RenderContext& context, unsigned int stride, const void *data, unsigned int count, BufferType bufferType);
		void createDynamicBuffer(RenderContext& context, unsigned int stride, const void* data, unsigned int count, BufferType bufferType);
        void resizeBuffer(RenderContext& context, const void* data, unsigned int size);
        void updateBuffer(RenderContext& context, unsigned int stride, void*& data, unsigned int count, MapType mapType = MAP_WRITE_DISCARD);
        bool isValid() const { return m_buffer; }

        MC_FUNC_MOVE(BufferD3D11);
	};
}